/*
 * findstate.cpp
 *
 *  Created on: 28/04/2013
 *      Author: Moya
 */

#include <iostream>

#include <causalize/state_variables_finder.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>
#include <util/debug.h>

StateVariablesFinder::StateVariablesFinder(MMO_Class c) {
	_c = c;
	_varSymbolTable = c->getVarSymbolTable();
}

StateVariablesFinder::~StateVariablesFinder() {
}

void StateVariablesFinder::findStateVariables() {
  MMO_EquationList eqs = _c->getEquations();
  findStateVariablesInEquations(eqs);
}

void StateVariablesFinder::findStateVariablesInEquations(MMO_EquationList eqs) {
  MMO_EquationListIterator eqIter;
  if (eqs != NULL) {
    foreach(eqIter, eqs) {
      MMO_Equation eq = current_element(eqIter);
      switch(eq->equationType()) {
        case EQEQUALITY:
        {
          AST_Equation_Equality eqEq = eq->getAsEquality();
          mapTraverse(eqEq->left());
          mapTraverse(eqEq->right());
        }
          break;
        case EQWHEN:
        {
          AST_Equation_When eqWhen = eq->getAsWhen();
          findStateVariablesInEquations(eqWhen->equationList());
          AST_Equation_ElseList elseWhenList = eqWhen->equationElseWhen();
          AST_Equation_ElseListIterator elseWhenListIter;
          foreach(elseWhenListIter, elseWhenList) {
            AST_Equation_Else eqElse = current_element(elseWhenListIter);
            findStateVariablesInEquations(eqElse->equations());
          }
        }
          break;
        default:
          ERROR("StateVariablesFinder::findStateVariablesInEquations:\n"
              "Equation type not supported.\n");
      }
    }
  }
}

AST_Expression StateVariablesFinder::mapTraverseElement(AST_Expression e) {
	switch(e->expressionType()) {
		case EXPDERIVATIVE:
			AST_Expression_Derivative der = e->getAsDerivative();
			AST_ExpressionList arguments = der->arguments();
			ERROR_UNLESS(arguments->size() == 1, "StateVariablesFinder::mapTraverseElement:\n"
			    "AST_Expression_Derivative with more than one argument are not supported yet.\n");
			AST_Expression argument = arguments->front();
			switch(argument->expressionType()) {
				case EXPCOMPREF:
				{
					AST_Expression_ComponentReference compref = argument->getAsComponentReference();
					VarInfo varInfo = _varSymbolTable->lookup(CREF_NAME(compref));
						varInfo->setState();
				}
					break;
				default:
				  ERROR("StateVariablesFinder::mapTraverseElement:\n"
				      "Incorrect expression type of AST_Expression_Derivative argument\n.");
			}

	}
	return e;
}

