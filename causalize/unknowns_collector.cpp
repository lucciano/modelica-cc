/*
 * collect_unknowns.cpp
 *
 *  Created on: 29/04/2013
 *      Author: fede
 */

#include <ast/ast_builder.h>
#include <causalize/unknowns_collector.h>
#include <util/symbol_table.h>
#include <iostream>

UnknownsCollector::UnknownsCollector(MMO_Class c) {
	_c = c;
	_unknowns = newAST_ExpressionList();
}

UnknownsCollector::~UnknownsCollector() {
}

AST_ExpressionList UnknownsCollector::collectUnknowns() {
  VarSymbolTable symbolTable = _c->getVarSymbolTable();
	int i; int symbolTableSize = symbolTable->count();
	for (i = 0; i<symbolTableSize; i++) {
		VarInfo varInfo = symbolTable->varInfo(i);
		if (varInfo->type()->getType() == TYREAL
				&& !varInfo->isConstant()
				&& !varInfo->isDiscrete()
				&& !varInfo->isParameter()) {
		    AST_Expression_ComponentReference compRef = (AST_Expression_ComponentReference) newAST_Expression_ComponentReferenceExp(newAST_String((string) symbolTable->varName(i)));
		    if (varInfo->isState()) {
				  AST_ExpressionList expList = newAST_ExpressionList(compRef);
				  _unknowns->push_back(newAST_Expression_Derivative(expList));
				} else {
				  _unknowns->push_back(compRef);
				}
		}
	}
	return _unknowns;
}
