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

#include <causalize/for_unrolling/for_index_iterator.h>
#include <ast/expression.h>
#include <ast/modification.h>
#include <util/ast_util.h>
#include <util/debug.h>

RangeIterator::RangeIterator(AST_Expression_Range range, VarSymbolTable symbolTable) {
  _rangeElements = range->expressionList();
  AST_ExpressionListIterator iter = _rangeElements->begin();
  _rangeBegin = eval(current_element(iter), symbolTable);
  iter++;
  AST_Real temp = eval(current_element(iter), symbolTable);
  iter++;
  if (iter == _rangeElements->end()) {
   _rangeStep = 1;
    _rangeEnd = temp;
  } else {
   _rangeStep = temp;
   _rangeEnd = eval(current_element(iter), symbolTable);
  }
  _current = _rangeBegin;
}

AST_Real RangeIterator::eval(AST_Expression exp, VarSymbolTable symbolTable) {
  EvalExp evaluator(symbolTable);
  AST_Expression result =  evaluator.eval(exp);
  ERROR_UNLESS(result->expressionType() == EXPREAL || result->expressionType() == EXPINTEGER, "RangeIterator::getVal:\n"
      "Expression type should be EXPREAL or EXPINTEGER \n");
  if (result->expressionType() == EXPREAL) {
    return result->getAsReal()->val();
  } else {
    return result->getAsInteger()->val();
  }
}

bool RangeIterator::hasNext() {
  return _current <= _rangeEnd;
}

AST_Real RangeIterator::next() {
  AST_Real value = _current;
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

AST_Real BraceIterator::next() {
  AST_Expression exp = *_braceExpElementsIter;
  AST_Real value;
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
