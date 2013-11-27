/*
 * CompRefOccurrenceTraverse.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/compref_occurrence.h>
#include <util/symbol_table.h>
#include <util/debug.h>

bool occur(AST_Expression unknown, MMO_Equation equation, VarSymbolTable symbolTable) {
  CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(unknown, symbolTable);
  switch (equation->equationType()) {
    case EQEQUALITY:
    {
      AST_Equation_Equality eqEquality = equation->getAsEquality();
      return occurrenceTraverse->foldTraverse(eqEquality->left()) ||
          occurrenceTraverse->foldTraverse(eqEquality->right());
    }
    default:
      ERROR("Equation type not supported.\n");
  }
  return false;
}

CompRefOccurrenceTraverse::CompRefOccurrenceTraverse(AST_Expression unknown, VarSymbolTable symbolTable) {
  _unknown = unknown;
  _equalExp = new EqualExp(symbolTable);
}

CompRefOccurrenceTraverse::~CompRefOccurrenceTraverse() {
}

bool CompRefOccurrenceTraverse::foldTraverseElement(AST_Expression e) {
	return _equalExp->equalTraverse(e, _unknown);
}


bool CompRefOccurrenceTraverse::foldTraverseElementUMinus(AST_Expression u) {
	return foldTraverse(u->getAsUMinus()->exp());
}

bool CompRefOccurrenceTraverse::foldTraverseElement(bool bool1, bool bool2, BinOpType enumBinOpType) {
  return bool1 || bool2;
}
