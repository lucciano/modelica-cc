#include <iostream>

#include <antialias/remove_alias.h>
#include <ast/ast_builder.h>
#include <ast/modification.h>
#include <ast/expression.h>
#include <util/symbol_table.h>
#include <util/ast_util.h>
#include <util/debug.h>
#include <causalize/state_variables_finder.h>

void RemoveAlias::addAlias(AST_Expression var, AST_Expression alias) {

  VarInfo v = _varSymbolTable->lookup(CREF_NAME(var));
  if (v==NULL) {
    cerr << "Unknown variable " << CREF_NAME(var)<< endl;
    return;
  }
  _varSymbolTable->remove(CREF_NAME(alias));
  AST_Comment c = v->comment();
  AST_String alias_name;
  if (IS_UMINUS(alias)) {
    alias_name = newAST_String(CREF_NAME(alias));
    alias_name->insert(0,"-");
  } else {
    alias_name = newAST_String(CREF_NAME(alias));
  }

  if (c!=NULL) {
    /* add to comment */
    AST_ArgumentList al = c->arguments();
    AST_ArgumentListIterator it;
    bool found=false;
    foreach(it,al) {
      if (*current_element(it)->getAsModification()->name()=="alias") {
        found=true;
        AST_ExpressionList l=current_element(it)->getAsModification()->modification()->getAsEqual()->exp()->getAsBrace()->arguments();
        AST_ListAppend(l,newAST_Expression_String(alias_name));
        break;
      }
    }
    if (!found)
      AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_Modification_Equal(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(alias_name))))));
  } else {
    AST_ArgumentList al= newAST_ArgumentList();
    AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_Modification_Equal(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(alias_name))))));
    c = newAST_Comment(NULL,al);
    v->setComment(c);
  }
  /* Do the replacement */
  MMO_EquationList eqs = _c->getEquations();
  MMO_EquationListIterator eqit;
  if (IS_UMINUS(alias) && !IS_UMINUS(var)) {
    var = newAST_Expression_UnaryMinus(var);
    alias = UMINUS_EXP(alias);
  } else  if (IS_UMINUS(alias) && IS_UMINUS(var)) {
    alias = UMINUS_EXP(alias);
    var = UMINUS_EXP(var);
  }
  if (eqs != NULL) {
    foreach(eqit, eqs)
    replaceExpInEq(alias,var,current_element(eqit));
  }
}

void RemoveAlias::replaceExpInEq(AST_Expression alias, AST_Expression var, AST_Equation eq) {
  ReplaceExp rep;
  switch(eq->equationType()) {
  case EQEQUALITY: {
    AST_Equation_Equality eqeq =  eq->getAsEquality();
    eqeq->setLeft(rep.replaceExp(alias,var,eqeq->left(), _varSymbolTable));
    eqeq->setRight(rep.replaceExp(alias,var,eqeq->right(), _varSymbolTable));
    break;
  }
  case EQWHEN: {
    AST_Equation_When eqwhen =  eq->getAsWhen();
    AST_Equation_ElseList elseeq=eqwhen->equationElseWhen();
    AST_Equation_ElseListIterator else_it;
    MMO_EquationListIterator eqit;
    MMO_EquationList eqs = eqwhen->equationList();
    eqwhen->setCondition(rep.replaceExp(alias,var,eqwhen->condition(), _varSymbolTable));
    foreach(eqit, eqs)
    replaceExpInEq(alias,var,current_element(eqit));
    foreach(else_it,elseeq) {
      current_element(else_it)->setCondition(rep.replaceExp(alias,var, current_element(else_it)->condition(), _varSymbolTable));
      foreach(eqit, current_element(else_it)->equations())
      replaceExpInEq(alias,var,current_element(eqit));
    }
    break;
  }
  case EQIF: {
    AST_Equation_If eqif =  eq->getAsIf();
    eqif->setCondition(rep.replaceExp(alias,var,eqif->condition(), _varSymbolTable));
    MMO_EquationListIterator eqit;
    MMO_EquationList eqs = eqif->equationList();
    AST_Equation_ElseList else_ls=eqif->equationElseIf();
    AST_Equation_ElseListIterator else_it;
    foreach(eqit, eqs)
    replaceExpInEq(alias,var,current_element(eqit));
    eqs = eqif->equationElseList();
    foreach(eqit, eqs)
    replaceExpInEq(alias,var,current_element(eqit));
    foreach(else_it, else_ls) {
      current_element(else_it)->setCondition(rep.replaceExp(alias,var,current_element(else_it)->condition(), _varSymbolTable));
      eqs = current_element(else_it)->equations();
      foreach(eqit, eqs)
      replaceExpInEq(alias,var,current_element(eqit));
    }
    break;
  }
  default:
    /* TODO */
    break;
  }
}
void RemoveAlias::removeAliasEquations(MMO_Class c) {
  _c = c;
  _varSymbolTable = _c->getVarSymbolTable();
  StateVariablesFinder *finder = new StateVariablesFinder(_c);
  finder->findStateVariables();
  MMO_EquationList eqs = _c->getEquations();
  MMO_EquationListIterator eqit;
  MMO_EquationList remove = newAST_EquationList();

  if (eqs != NULL) {
    foreach(eqit, eqs) {
      AST_Equation eq = (AST_Equation) current_element(eqit);
      switch(eq->equationType()) {
      case EQEQUALITY:
        AST_Equation_Equality eqeq =  eq->getAsEquality();
        AST_Expression left=eqeq->left(),right=eqeq->right();
        if (IS_ZERO(left) && IS_SUM_OF_VARS(right)) {
          // 0 =a + b;
          AST_Expression binop=right;
          if (IS_ADD(binop)) {
            eqeq->setLeft(newAST_Expression_UnaryMinus(LEFT_EXP(binop)));
            eqeq->setRight(RIGHT_EXP(binop));
          } else {
            eqeq->setLeft(LEFT_EXP(binop));
            eqeq->setRight(RIGHT_EXP(binop));
          }
          left=eqeq->left();
          right=eqeq->right();
          delete binop;
        } else if (IS_ZERO(right) && IS_SUM_OF_VARS(left)) {
          // a + b = 0;
          AST_Expression binop=left;
          if (IS_ADD(binop)) {
            eqeq->setLeft(newAST_Expression_UnaryMinus(LEFT_EXP(binop)));
            eqeq->setRight(RIGHT_EXP(binop));
          } else {
            eqeq->setLeft(LEFT_EXP(binop));
            eqeq->setRight(RIGHT_EXP(binop));
          }
          left=eqeq->left();
          right=eqeq->right();
          delete binop;
        }
        /*
        if (IS_VAR(eqeq->left()) && ic.foldTraverse(eqeq->right())) {
          // a = const;
          cerr << "REMOVING CONST_ALIAS: "<< eqeq;
         AST_ListAppend(remove,(AST_Equation)eqeq);
        }
        if (IS_VAR(eqeq->right()) && ic.foldTraverse(eqeq->left())) {
          // const =a;
          cerr << "REMOVING CONST_ALIAS: "<< eqeq;
         AST_ListAppend(remove,(AST_Equation)eqeq);
        }
        */
        if (IS_VAR(left) && IS_VAR(right)) {
          if (!IS_STATE(left)) {
            DEBUG('a' ,"ADDING ALIAS %s FOR %s from eq %s",left->print().c_str(), right->print().c_str(),eqeq->print().c_str());
            addAlias(right,left);
            AST_ListAppend(remove,static_cast<AST_Equation>(eqeq));
          } else if (!IS_STATE(right)) {
            DEBUG('a' ,"ADDING ALIAS %s FOR %s from eq %s",left->print().c_str(), right->print().c_str(),eqeq->print().c_str());
            addAlias(left,right);
            AST_ListAppend(remove,static_cast<AST_Equation>(eqeq));
          }
        }
        break;
      }
    }
    foreach(eqit, remove)
    _c->removeEquation(current_element(eqit));
  }
}


