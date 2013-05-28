#include <iostream>

#include <antialias/remove_alias.h>
#include <ast/ast_builder.h>
#include <ast/modification.h>
#include <ast/expression.h>
#include <util/symbol_table.h>
#include <util/ast_util.h>
#include <causalize/find_state.h>

void RemoveAlias::addAlias(AST_Expression var, AST_Expression alias) {
  
  VarInfo *v = _varSymbolTable->lookup(CREF_NAME(var));
  if (v==NULL) {
    cerr << "Unknown variable " << CREF_NAME(var)<< endl;
    return;
  } 
  _varSymbolTable->remove(CREF_NAME(alias));
  AST_Modification m = v->modification();
  AST_String alias_name;
  if (IS_UMINUS(alias)) {
    alias_name = newAST_String(CREF_NAME(alias));
    alias_name->insert(0,"-");
  } else {
    alias_name = newAST_String(CREF_NAME(alias));
  }
 
  if (m!=NULL) {
    /* add to modificaiton */
    switch (m->modificationType()) {
      case MODCLASS: {
        AST_ArgumentList al = m->getAsModificationClass()->arguments();
        AST_ArgumentListIterator it;
        bool found=false;
        foreach(it,al) { 
          if (*current(it)->getAsArgumentModification()->name()=="alias") { 
            AST_ExpressionList l=current(it)->getAsArgumentModification()->modification()->getAsModificationEqual()->exp()->getAsExpression_Brace()->arguments();
            found=true;
            AST_ListAppend(l,newAST_Expression_String(alias_name));
            break;
          } 
        }
        if (!found) 
            AST_ListAppend(m->getAsModificationClass()->arguments(),newAST_ElementModification(newAST_String("alias"),newAST_ModificationEqual(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(alias_name))))));
        break;
        }
        case MODEQUAL: {
          AST_ModificationEqual me=m->getAsModificationEqual(); 
          AST_ArgumentList al= newAST_ArgumentList();
          AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_ModificationEqual(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(alias_name))))));
          m = newAST_ModificationClass(al, me->exp());
          v->setModification(m);
          break;
        }
      }
  } else {
    AST_ArgumentList al= newAST_ArgumentList();
  AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_ModificationEqual(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(alias_name))))));
   m = newAST_ModificationClass(al, newAST_Expression_Null());
    v->setModification(m); 
  }
  /* Do the replacement */ 
  ReplaceExp rep;
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
    foreach(eqit, eqs) {
      AST_Equation eq = current(eqit);
      switch(eq->equationType()) {
        case EQEQUALITY:
	        AST_Equation_Equality eqeq =  eq->getAsEquality();
          eqeq->setLeft(rep.replaceExp(alias,var,eqeq->left()));
          eqeq->setRight(rep.replaceExp(alias,var,eqeq->right()));
          break;
      }
    }
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
      AST_Equation eq = (AST_Equation) current(eqit);
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
          left=eqeq->left();right=eqeq->right();
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
          left=eqeq->left();right=eqeq->right();
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
          if (!IS_STATE(CREF_NAME(left))) {
            cerr << "ADDING ALIAS " << left << " FOR " << right << " FROM ALIAS EQ: " << eqeq;
            addAlias(right,left);
            AST_ListAppend(remove,(AST_Equation)eqeq);
          } else if (!IS_STATE(CREF_NAME(right))) {
            cerr << "ADDING ALIAS " << right << " FOR " << left << " FROM ALIAS EQ: " << eqeq;
            addAlias(left,right);
            AST_ListAppend(remove,(AST_Equation)eqeq);
          }
        }
	      break;
  	  }
    }
    foreach(eqit, remove) 
      _c->removeEquation(current(eqit));  
  }
}


