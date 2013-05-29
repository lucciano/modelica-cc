/*
 * findstate.cpp
 *
 *  Created on: 28/04/2013
 *      Author: Moya
 */

#include <iostream>

#include <causalize/find_state.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/symbol_table.h>

StateVariablesFinder::StateVariablesFinder(MMO_Class c) {
	_c = c;
	_varSymbolTable = c->getVarSymbolTable();
}

StateVariablesFinder::~StateVariablesFinder() {
	// TODO [Moya] Auto-generated destructor stub
}

void StateVariablesFinder::findStateVariables() {
  MMO_EquationList eqs = _c->getEquations();
  findStateVariablesInEquations(eqs);
}

void StateVariablesFinder::findStateVariablesInEquations(MMO_EquationList eqs) {
  MMO_EquationListIterator eqIter;
  if (eqs != NULL) {
    foreach(eqIter, eqs) {
      MMO_Equation eq = current(eqIter);
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
            AST_Equation_Else eqElse = current(elseWhenListIter);
            findStateVariablesInEquations(eqElse->equations());
          }
        }
          break;
        case EQCALL:
          // TODO [Moya] Este caso no hay que considerarlo, cierto?
          break;
        case EQCONNECT:
          // TODO [Moya]
          break;
        case EQFOR:
          // TODO [Moya]
          break;
        case EQIF:
          // TODO Es correcto considerar toda la lista de ecuaciones? Porque de este if sale una sola ecuacion, o no?
//          AST_Equation_If eqIf = eq->getAsIf();
//          AST_EquationList innerEqs = eqIf->equationList();
//          findStateVariablesInEquations(innerEqs);
          break;
        case EQNONE:
          // TODO [Moya] ver como es el manejo de errores y/o excepciones.
          break;
        default:
        ;// TODO [Moya] ver como es el manejo de errores y/o excepciones.
      }
    }
  }
}

AST_Expression StateVariablesFinder::mapTraverseElement(AST_Expression e) {
	switch(e->expressionType()) {
		case EXPDERIVATIVE:
			AST_Expression_Derivative der = e->getAsDerivative();
			AST_ExpressionList arguments = der->arguments();
			AST_Expression argument = arguments->front(); // FIXME [Moya] Por ahora nos quedamos con el 1er elemento. En un futuro habra que iterar sobre todos los elementos de la lista.
			switch(argument->expressionType()) {
				case EXPCOMPREF:
				{
					AST_Expression_ComponentReference compref = argument->getAsComponentRef();
					VarInfo varInfo = _varSymbolTable->lookup(compref->name());
					if (varInfo != NULL) {
						varInfo->setState();
						//cout << compref->name() << endl;
					} else {
						// TODO [Moya]
					}
				}
					break;
				// TODO [Moya] ver como es el manejo de errores y/o excepciones.
			}

	}
	return e;
}

