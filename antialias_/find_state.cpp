/*
 * findstate.cpp
 *
 *  Created on: 28/04/2013
 *      Author: fede
 */

#include <iostream>

#include <causalize/find_state.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>

StateVariablesFinder::StateVariablesFinder(MMO_Class *c) {
	_c = c;
	_varSymbolTable = c->getVarSymbolTable();
}

StateVariablesFinder::~StateVariablesFinder() {
	// TODO [Fede] Auto-generated destructor stub
}

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

void StateVariablesFinder::findStateVariables() {

	MMO_EquationList eqs = _c->getEquations();
	MMO_EquationListIterator eqit;

		if (eqs != NULL) {
			foreach(eqit, eqs) {
				AST_Equation eq = (AST_Equation) current(eqit);
				switch(eq->equationType()) {
					case EQEQUALITY:
						AST_Equation_Equality eqeq =  eq->getAsEquality();
						mapTraverse(eqeq->left());
						mapTraverse(eqeq->right());
                                                IsConstant ic(_varSymbolTable);
                                                if (IS_VAR(eqeq->left()) && IS_VAR(eqeq->right())) {
                                                  // a = b;
                                                  cerr << "ALIAS: "<< eqeq;
                                                }
                                                if (IS_ZERO(eqeq->left()) && IS_SUM_OF_VARS(eqeq->right())) {
                                                  // 0 =a + b;
                                                  cerr << "SOLV_ALIAS: "<< eqeq;
                                                  if (IS_ADD(eqeq->right())) {
                                                    cerr << newAST_Expression_UnaryMinus(LEFT_EXP(eqeq->right())) << " = " << RIGHT_EXP(eqeq->right()) << endl; 
                                                  } else { 
                                                    cerr << LEFT_EXP(eqeq->right()) << " = " << RIGHT_EXP(eqeq->right()) << endl; 
                                                  }
                                                }
                                                if (IS_ZERO(eqeq->right()) && IS_SUM_OF_VARS(eqeq->left())) {
                                                  // a + b = 0;
                                                  cerr << "SOLV_ALIAS: "<< eqeq;
                                                  if (IS_ADD(eqeq->left())) {
                                                    cerr << newAST_Expression_UnaryMinus(LEFT_EXP(eqeq->left())) << " = " << RIGHT_EXP(eqeq->left()) << endl; 
                                                  } else {
                                                    cerr << LEFT_EXP(eqeq->left()) << " = " << RIGHT_EXP(eqeq->left()) << endl; 
                                                  }
                                                }
                                                if (IS_VAR(eqeq->left()) && ic.foldTraverse(eqeq->right())) {
                                                  // a = const;
                                                  cerr << "CONST_ALIAS: "<< eqeq;
                                                }
                                                if (IS_VAR(eqeq->right()) && ic.foldTraverse(eqeq->left())) {
                                                  // const =a;
                                                  cerr << "CONST_ALIAS: "<< eqeq;
                                                }
                                                  
						break;
					// TODO [Fede] ver como es el manejo de errores y/o excepciones.
				}
			}
		}
}

AST_Expression StateVariablesFinder::mapTraverseElement(AST_Expression e) {
	switch(e->expressionType()) {
		case EXPDERIVATIVE:
			AST_Expression_Derivative der = e->getAsDerivative();
			AST_ExpressionList arguments = der->arguments();
			AST_Expression argument = arguments->front(); // FIXME [Fede] Por ahora nos quedamos con el 1er elemento. En un futuro habra que iterar sobre todos los elementos de la lista.
			switch(argument->expressionType()) {
				case EXPCOMPREF:
					AST_Expression_ComponentReference compref = argument->getAsComponentRef();
					VarInfo *varInfo = _varSymbolTable->lookup(compref->name());
					if (varInfo != NULL) {
						varInfo->setState();
					} else {
						// TODO [Fede]
					}
					break;
				// TODO [Fede] ver como es el manejo de errores y/o excepciones.
			}

	}
	return e;
}

