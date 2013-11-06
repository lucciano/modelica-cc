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

#include <math.h>
#include <ast/ast_builder.h>
#include <util/debug.h>
#include <causalize/for_unrolling/instantiation_fold.h>

AST_Expression InstantiationFold::instantiate(AST_String variable, AST_Integer index, AST_Expression exp) {
  _forIndex_variable = variable;
  _forIndex = index;
  return foldTraverse(exp);
}

AST_Expression InstantiationFold::foldTraverseElement(AST_Expression exp) {
  switch (exp->expressionType()) {
  case EXPCOMPREF: {
    AST_Expression_ComponentReference expCompRef = exp->getAsComponentReference();
    ERROR_UNLESS(expCompRef->indexes()->size() >= 1, "InstantiationTraverse::mapTraverseElement:\n"
        "Wrong Component reference index list size: %d.\n", expCompRef->indexes()->size());
    if (expCompRef->indexes()->size() == 1 && expCompRef->indexes()->front()->size() == 0) {
      return instantiateCompRef(expCompRef);
    } else {
      return instantiateArray(expCompRef);
    }
  } case EXPDERIVATIVE: {
    AST_Expression_Derivative der = exp->getAsDerivative();
    AST_ExpressionList arguments = der->arguments();
    AST_ExpressionListIterator argsIter;
    AST_ExpressionList newArgs = newAST_ExpressionList();
    foreach(argsIter, arguments) {
      AST_Expression arg = current_element(argsIter);
      ERROR_UNLESS(arg->expressionType() == EXPCOMPREF, "InstantiationTraverse::mapTraverseElement:\n"
          "Incorrect AST_ExpressionDerivative argument type or not supported yet.\n");
      AST_Expression newArg = instantiateArray((AST_Expression_ComponentReference)arg);
      newArgs->push_back(newArg);
    }
    return newAST_Expression_Derivative(newArgs);
  } default:
    return exp;
  }
}

AST_Expression InstantiationFold::foldTraverseElementUMinus(AST_Expression exp) {
  return foldTraverse(exp->getAsUMinus()->exp());
}

AST_Expression InstantiationFold::foldTraverseElement(AST_Expression left, AST_Expression right, BinOpType binOpType) {
  switch(binOpType){
  case BINOPADD: {
    if (isNumericExpression(left) &&
        isNumericExpression(right)) {
      return newAST_Expression_Integer(getNumericExpressionVal(left) + getNumericExpressionVal(right));
    } else {
      return newAST_Expression_BinOp(left, right, BINOPADD);
    }
  } case BINOPSUB: {
    if (isNumericExpression(left) &&
        isNumericExpression(right)) {
      return newAST_Expression_Integer(getNumericExpressionVal(left) - getNumericExpressionVal(right));
    } else {
      return newAST_Expression_BinOp(left, right, BINOPSUB);
    }
  } case BINOPMULT: {
    if (isNumericExpression(left) &&
        isNumericExpression(right)) {
      return newAST_Expression_Integer(getNumericExpressionVal(left) * getNumericExpressionVal(right));
    } else {
      return newAST_Expression_BinOp(left, right, BINOPMULT);
    }
  } case BINOPDIV: {
    ERROR_UNLESS(right != 0, "process_for_equations - evalExp:\n"
            "Division by zero.\n");
    if (isNumericExpression(left) &&
        isNumericExpression(right)) {
      return newAST_Expression_Integer((int)(getNumericExpressionVal(left) / getNumericExpressionVal(right)));
    } else {
      return newAST_Expression_BinOp(left, right, BINOPDIV);
    }
  } case BINOPEXP: {
    if (isNumericExpression(left) &&
        isNumericExpression(right)) {
      return newAST_Expression_Integer((int) pow(getNumericExpressionVal(left), getNumericExpressionVal(right)));
    } else {
      return newAST_Expression_BinOp(left, right, BINOPEXP);
    }
  } default:
    ERROR("process_for_equations.cpp - evalBinOp:\n"
        "Incorrect Binary operation type.\n");
  }
}

AST_Expression InstantiationFold::instantiateCompRef(AST_Expression_ComponentReference compRef) {
  if (compRef->name().compare(*_forIndex_variable) == 0) {
    return newAST_Expression_Integer(_forIndex);
  } else {
    return compRef;
  }
}

// The component reference is an array of the form
// a[i,j].b[x].c[y] (indexesList is ( (i,j) , (x), (y) ).
AST_Expression InstantiationFold::instantiateArray(AST_Expression_ComponentReference array) {
  AST_Expression_ComponentReference newArray = newAST_Expression_ComponentReference();
  AST_StringList names = array->names();
  AST_StringListIterator namesIter = names->begin();
  AST_ExpressionListList indexesList = array->indexes();
  AST_ExpressionListListIterator indexesListIter;
  foreach (indexesListIter, indexesList) {
    AST_ExpressionListIterator listIter;
    AST_ExpressionList indexes = current_element(indexesListIter);
    AST_ExpressionList newIndexes = newAST_ExpressionList();
    foreach (listIter, indexes) {
      AST_Expression arrayIndex = current_element(listIter);
      AST_Expression newArrayIndex = foldTraverse(arrayIndex);
      newIndexes->push_back(newArrayIndex);
    }
    newArray->append((AST_String)current_element(namesIter), newIndexes);
    namesIter++;
  }
  return newArray;
}

bool InstantiationFold::isNumericExpression(AST_Expression exp) {
  return exp->expressionType() == EXPINTEGER || exp->expressionType() == EXPREAL;
}

AST_Real InstantiationFold::getNumericExpressionVal(AST_Expression exp) {
  switch (exp->expressionType()) {
  case EXPINTEGER: {
    AST_Expression_Integer integer = exp->getAsInteger();
    return integer->val();
  } case EXPREAL: {
    AST_Expression_Real real = exp->getAsReal();
    return real->val();
  } default:
      ERROR("InstantiationFold::getNumericExpressionVal: \n"
            "Incorrect type %d.\n", exp->expressionType());
  }
}
