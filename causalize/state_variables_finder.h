/*
 * findstate.h
 *
 *  Created on: 28/04/2013
 *      Author: fede
 */

#include <iostream>
#include <mmo/mmo_class.h>
#include <util/ast_util.h>
#include <util/symbol_table.h>

using namespace std;

class StateVariablesFinder : public AST_Expression_Traverse {
public:
	StateVariablesFinder(MMO_Class c);
	virtual ~StateVariablesFinder();
	void findStateVariables();
	virtual AST_Expression mapTraverseElement(AST_Expression);
private:
	void findStateVariablesInEquations(MMO_EquationList eqs);
	MMO_Class _c;
	VarSymbolTable _varSymbolTable;
};

