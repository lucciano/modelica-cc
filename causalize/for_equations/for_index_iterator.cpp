/*****************************************************************************

    This file is part of Modelica C Compiler.

    Modelica C Compiler is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Modelica C Compiler is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Modelica C Compiler.  If not, see <http://www.gnu.org/licenses/>.

******************************************************************************/

#include <causalize/for_equations/for_index_iterator.h>
#include <ast/expression.h>
#include <util/debug.h>

RangeIterator::RangeIterator(AST_Expression_Range range) {
  _rangeElements = range->expressionList();
  AST_ExpressionListIterator iter = _rangeElements->begin();
  if((*iter)->expressionType() == EXPREAL) {
    initReal(iter);
  } else {
    initInteger(iter);
  }
  _current = _rangeBegin;
}

void RangeIterator::initReal(AST_ExpressionListIterator iter){
  _rangeBegin = (*iter)->getAsReal()->val();
  iter++;
  if (iter == _rangeElements->end()) {
    _rangeStep = 1.0;
   _rangeEnd = (*iter)->getAsReal()->val();
  } else {
   _rangeStep = (*iter)->getAsReal()->val();
   iter++;
   ERROR_UNLESS(iter == _rangeElements->end(), "for_index_iterator - getIndexList:\n"
       "Incorrect AST_Expression_Range\n");
   _rangeEnd = (*iter)->getAsReal()->val();
  }
}

void RangeIterator::initInteger(AST_ExpressionListIterator iter){
  _rangeBegin = (*iter)->getAsInteger()->val();
  iter++;
  if (iter == _rangeElements->end()) {
   _rangeEnd = (*iter)->getAsInteger()->val();
  } else {
   _rangeStep = (*iter)->getAsInteger()->val();
   iter++;
   ERROR_UNLESS(iter == _rangeElements->end(), "for_index_iterator - getIndexList:\n"
       "Incorrect AST_Expression_Range\n");
   _rangeEnd = (*iter)->getAsInteger()->val();
  }
}

bool RangeIterator::hasNext() {
  return _current <= _rangeEnd - _rangeStep;
}

AST_Integer RangeIterator::next() {
  AST_Integer value = (int) _current;
  _current += _rangeStep;
  return value;
}

BraceIterator::BraceIterator(AST_Expression_Brace braceExp) {
  _braceExpElements = braceExp->arguments();
  _braceExpElementsIter = _braceExpElements->begin();
}

bool BraceIterator::hasNext() {
  return _braceExpElementsIter == _braceExpElements->end();
}

AST_Integer BraceIterator::next() {
  AST_Expression exp = *_braceExpElementsIter;
  AST_Integer value;
  switch (exp->expressionType()){
  case EXPINTEGER:
    value = exp->getAsInteger()->val();
    break;
  case EXPREAL:
    value = (int)exp->getAsReal()->val();
    break;
  default:
    ERROR("BraceIterator::next():\n"
        "Incorrect brace expression element's type.");
  }
  _braceExpElementsIter++;
  return value;
}
