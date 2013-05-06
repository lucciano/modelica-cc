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
						cout << compref->name() << endl;
					} else {
						// TODO [Fede]
					}
					break;
				// TODO [Fede] ver como es el manejo de errores y/o excepciones.
			}

	}
	return e;
}

