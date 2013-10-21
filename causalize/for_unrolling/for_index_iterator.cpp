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
#include <util/debug.h>


RangeIterator::RangeIterator(AST_Expression_Range range, VarSymbolTable symbolTable) {
  _rangeElements = range->expressionList();
  AST_ExpressionListIterator iter = _rangeElements->begin();
  _rangeBegin = getVal(iter, symbolTable);
  iter++;
  int temp = getVal(iter, symbolTable);
  iter++;
  if (iter == _rangeElements->end()) {
   _rangeStep = 1;
    _rangeEnd = temp;
  } else {
   _rangeStep = temp;
   _rangeEnd = getVal(iter, symbolTable);
  }
  _current = _rangeBegin;
}

AST_Real RangeIterator::getVal(AST_ExpressionListIterator iter, VarSymbolTable symbolTable) {
  AST_Expression exp = current_element(iter);
  switch(exp->expressionType()) {
  case EXPINTEGER:
    return exp->getAsInteger()->val();
  case EXPREAL:
    return exp->getAsReal()->val();
  case EXPCOMPREF:
    return getCompRefVal(exp->getAsComponentReference(), symbolTable);
  default:
    ERROR("RangeIterator::getVal:\n"
        "Incorrect colon expression element's type");
  }
}

AST_Real RangeIterator::getCompRefVal(AST_Expression_ComponentReference compRef, VarSymbolTable symbolTable) {
  VarInfo vInfo = symbolTable->lookup(compRef->name());
  ERROR_UNLESS(vInfo->isConstant() || vInfo->isParameter(),
      "RangeIterator::getCompRefVal\n"
      "AST_Component_Reference in AST_Expression_Range must be constant or parameter.\n");
  AST_Modification mod = vInfo->modification();
  switch(mod->modificationType()) {
  case MODEQUAL:{
    AST_Modification_Equal equal = mod->getAsEqual();
    AST_Expression exp = equal->exp();
    switch (exp->expressionType()) {
    case EXPINTEGER:
      return exp->getAsInteger()->val();
    case EXPREAL:
      return exp->getAsReal()->val();
    default:
      // TODO [Moya]
      ERROR("RangeIterator::getCompRefVal\n"
          "For now only literals are supported as AST_Modification_Equal expression\n");
    }
    break;
  } default:
    ERROR("RangeIterator::getVal\n"
        "Incorrect AST_Modification type or not supported yet.\n");
  }
}

bool RangeIterator::hasNext() {
  return _current <= _rangeEnd;
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
