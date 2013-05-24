#include <iostream>

#include <antialias/remove_alias.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>
#include <util/ast_util.h>
#include <causalize/find_state.h>

void RemoveAlias::addAlias(AST_Expression a, AST_Expression b) {
 VarInfo *v  = _varSymbolTable->lookup(CREF_NAME(a));
  if (v==NULL) {
    cerr << "Unknown variable " << CREF_NAME(b)<< endl;
    return;
  } 
  _varSymbolTable->remove(CREF_NAME(b));
  AST_Modification m = v->modification();
 if (m!=NULL) {
  /* add to modificaiton */
 } else {
  AST_ArgumentList al= newAST_ArgumentList();
  AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_ModificationEqual(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(newAST_String(CREF_NAME(b))))))));
  m = newAST_ModificationClass(al, newAST_Expression_Null());
  v->setModification(m); 
  }
  /* Do the replacement */ 
  ReplaceExp rep;
  MMO_EquationList eqs = _c->getEquations();
  MMO_EquationListIterator eqit;
  if (eqs != NULL) {
    foreach(eqit, eqs) {
      AST_Equation eq = current(eqit);
      switch(eq->equationType()) {
        case EQEQUALITY:
	        AST_Equation_Equality eqeq =  eq->getAsEquality();
          eqeq->setLeft(rep.replaceExp(b,a,eqeq->left()));
          eqeq->setRight(rep.replaceExp(b,a,eqeq->right()));
          break;
      }
    }
  }
}

void RemoveAlias::removeAliasEquations(MMO_Class *c) {
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
        }
        if (IS_ZERO(right) && IS_SUM_OF_VARS(left)) {
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
        /*if (IS_VAR(eqeq->left()) && ic.foldTraverse(eqeq->right())) {
          // a = const;
          cerr << "REMOVING CONST_ALIAS: "<< eqeq;
         AST_ListAppend(remove,(AST_Equation)eqeq);
        }
        if (IS_VAR(eqeq->right()) && ic.foldTraverse(eqeq->left())) {
          // const =a;
          cerr << "REMOVING CONST_ALIAS: "<< eqeq;
         AST_ListAppend(remove,(AST_Equation)eqeq);
        }*/
        if (IS_VAR(left) && IS_VAR(right)) {
          if (IS_CREF(left) && !IS_STATE(left) && IS_CREF(right)) {
            // a = b;
            addAlias(right,left);
            cerr << "ADDING ALIAS " << CREF_NAME(left) << " FOR " << CREF_NAME(right) << " FROM ALIAS EQ: " << eqeq;
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


