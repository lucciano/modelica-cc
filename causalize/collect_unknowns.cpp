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
	_unknowns = new list<string>;
}

UnknownCollector::~UnknownCollector() {
}

list<string> *UnknownCollector::collectUnknowns() {
  VarSymbolTable symbolTable = _c->getVarSymbolTable();
	int i; int symbolTableSize = symbolTable->count();
	for (i = 0; i<symbolTableSize; i++) {
		VarInfo *varInfo = symbolTable->varInfo(i);
		if (varInfo->type()->getType() == TYREAL
				&& !varInfo->isConstant()
				&& !varInfo->isDiscrete()
				&& !varInfo->isParameter()) {
				if (varInfo->isState()) {
				  _unknowns->push_back(replaceStateByDer(symbolTable->varName(i)));
				} else {
				  _unknowns->push_back(symbolTable->varName(i));
				}
		}
	}
	return _unknowns;
}

string UnknownCollector::replaceStateByDer(string state) {
  string s1 =  "der(";
  string s2 = ")";
  return s1.append(state.append(s2));
}
