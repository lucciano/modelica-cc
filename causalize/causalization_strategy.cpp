/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/causalization_strategy.h>
#include <ast/equation.h>
#include <causalize/compref_occurrence_traverse.h>

#include <iostream>

CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations,
		AST_ExpressionList unknowns) {

	// TODO [Moya] Verificar #equations = # unknowns

	_equations = equations;
	_unknowns = unknowns;
	_N = equations->size();
	_equationByIndex = new MMO_Equation[_N];
	MMO_EquationListIterator eqIter;
	int i = 0;
	foreach(eqIter, equations) {
		MMO_Equation eq = current(eqIter);
		_equationByIndex[i++] = eq;
	}
	_expressionByIndex = new AST_Expression[_N];
	AST_ExpressionListIterator expIter;
	  int j = 0;
	  foreach(expIter, _unknowns) {
	    AST_Expression exp = current(expIter);
	    _expressionByIndex[j++] = exp;
	}
}

CausalizationStrategy::~CausalizationStrategy() {
	delete [] _equationByIndex;
}

MMO_EquationList CausalizationStrategy::causalize() {
	AST_ExpressionListIterator expIter;
  foreach(expIter, _unknowns) {
		AST_Expression unknown = current(expIter);
		for(int i=0; i<_N; i++){
			if (occur(unknown, _equationByIndex[i])){
				cout << _equationByIndex[i] << " " << unknown << endl;
			}
		}
	}
  return NULL;
}

bool CausalizationStrategy::occur(AST_Expression unknown, MMO_Equation equation) {
  CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(unknown);
  switch (equation->equationType()) {
    case EQEQUALITY:
    {
      AST_Equation_Equality eqEquality = equation->getAsEquality();
      return occurrenceTraverse->foldTraverse(eqEquality->left()) || occurrenceTraverse->foldTraverse(eqEquality->right());
    }
    default:
      ;// TODO [MOYA]
  }
  return false;
}
