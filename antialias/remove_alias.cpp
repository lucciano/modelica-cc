#include <iostream>

#include <antialias/remove_alias.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>
#include <causalize/find_state.h>

#define CREF_NAME(X) ((X)->getAsComponentRef()->name())
#define IS_CREF(X) ((X)->expressionType()==EXPCOMPREF)
#define IS_UMINUS(X) ((X)->expressionType()==EXPUMINUS)
#define IS_UMINUS_VAR(X) (IS_UMINUS(X) && IS_CREF((X)->getAsUMinus()->exp()))
#define IS_VAR(X) (IS_CREF(X) || IS_UMINUS_VAR(X))
#define IS_ZERO_REAL(X) ((X)->expressionType()==EXPREAL && (X)->getAsReal()->val()==0.0)
#define IS_ZERO_INT(X) ((X)->expressionType()==EXPINTEGER && (X)->getAsInteger()->val()==0)
#define IS_ZERO(X) (IS_ZERO_REAL(X) || IS_ZERO_INT(X))
#define IS_ADD(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPADD)
#define LEFT_EXP(X) ((X)->getAsBinOp()->left())
#define RIGHT_EXP(X) ((X)->getAsBinOp()->right())
#define IS_SUB(X) ((X)->expressionType()==EXPBINOP && (X)->getAsBinOp()->binopType()==BINOPSUB)
#define IS_SUM_(X) (IS_SUB(X) || IS_ADD(X))
#define IS_SUM_OF_VARS(X) (IS_SUM_(X) && (IS_VAR((X)->getAsBinOp()->left()) && IS_VAR((X)->getAsBinOp()->right())))
#define IS_STATE(X) (_varSymbolTable->lookup((X)->getAsComponentRef()->name())!=NULL && _varSymbolTable->lookup(X->getAsComponentRef()->name())->isState())

void RemoveAlias::addAlias(AST_Expression a, AST_Expression b) {
 VarInfo *v  = _varSymbolTable->lookup(CREF_NAME(a));
  if (v==NULL) {
    cerr << "Unknown variable " << CREF_NAME(b)<< endl;
    return;
  } 
  //_varSymbolTable->remove(CREF_NAME(b));
  AST_Modification m = v->modification();
 if (m!=NULL) {
  /* add to modificaiton */
 } else {
  AST_ArgumentList al= newAST_ArgumentList();
  AST_ListAppend(al,newAST_ElementModification(newAST_String("alias"),newAST_ModificationEqual(newAST_Expression_Brace(newAST_ExpressionList(newAST_Expression_String(newAST_String(CREF_NAME(b))))))));
  m = newAST_ModificationClass(al, newAST_Expression_Null());
  v->setModification(m); 
  }
 
}

void RemoveAlias::removeAliasEquations(MMO_Class *_c) {

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
           
            
            //cerr << "REMOVE ALIAS VAR: "<< eqeq->left()->getAsComponentRef()->name() << endl;
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


