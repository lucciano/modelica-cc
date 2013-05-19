/*
 * collect_unknowns.cpp
 *
 *  Created on: 29/04/2013
 *      Author: fede
 */

#include <causalize/collect_unknowns.h>
#include <util/symbol_table.h>
#include <iostream>

UnknownCollector::UnknownCollector(MMO_Class *c) {
	_c = c;
}

UnknownCollector::~UnknownCollector() {
}

void UnknownCollector::collectUnknowns() {
	cout << "Unknowns: ";
  VarSymbolTable symbolTable = _c->getVarSymbolTable();
	int i; int symbolTableSize = symbolTable->count();
	for (i = 0; i<symbolTableSize; i++) {
		VarInfo *varInfo = symbolTable->varInfo(i);
		if (varInfo->type()->getType() == TYREAL
				&& !varInfo->isConstant()
				&& !varInfo->isDiscrete()
				&& !varInfo->isParameter()) {
				if (varInfo->isState()) {
				  cout << "der(" << symbolTable->varName(i) << ")" << " ";
				} else {
				  cout << symbolTable->varName(i) << " ";
				}
		}
	}
	cout << endl;

}
