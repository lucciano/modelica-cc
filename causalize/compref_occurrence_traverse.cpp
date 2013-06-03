/*
 * CompRefOccurrenceTraverse.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include "compref_occurrence_traverse.h"

CompRefOccurrenceTraverse::CompRefOccurrenceTraverse(AST_Expression unknown) {
  _unknown = unknown;
}

CompRefOccurrenceTraverse::~CompRefOccurrenceTraverse() {
	// TODO Auto-generated destructor stub
}

bool CompRefOccurrenceTraverse::foldTraverseElement(AST_Expression e) {
	return EqualExp::equalTraverse(e, _unknown);
}

bool CompRefOccurrenceTraverse::foldTraverseElement(bool bool1, bool bool2, BinOpType enumBinOpType) {
  return bool1 || bool2;
}
