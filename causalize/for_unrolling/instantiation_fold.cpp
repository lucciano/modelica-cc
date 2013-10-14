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
  AST_Expression leftResult;
  AST_Expression rightResult;
  switch(binOpType){
  case BINOPADD: {
    leftResult = foldTraverse(left);
    rightResult = foldTraverse(right);
    if (leftResult->expressionType() != EXPCOMPREF &&
        rightResult->expressionType() != EXPCOMPREF) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) + getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPADD);
    }
  } case BINOPSUB: {
    leftResult = foldTraverse(left);
    rightResult = foldTraverse(right);
    if (leftResult->expressionType() != EXPCOMPREF &&
        rightResult->expressionType() != EXPCOMPREF) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) - getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPSUB);
    }
  } case BINOPMULT: {
    leftResult = foldTraverse(left);
    rightResult = foldTraverse(right);
    if (leftResult->expressionType() != EXPCOMPREF &&
        rightResult->expressionType() != EXPCOMPREF) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) * getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPMULT);
    }
  } case BINOPDIV: {
    leftResult = foldTraverse(left);
    rightResult = foldTraverse(right);
    ERROR_UNLESS(rightResult != 0, "process_for_equations - evalExp:\n"
            "Division by zero.\n");
    if (leftResult->expressionType() != EXPCOMPREF &&
        rightResult->expressionType() != EXPCOMPREF) {
      return newAST_Expression_Integer((int)(getNumericExpressionVal(leftResult) / getNumericExpressionVal(rightResult)));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPDIV);
    }
  } case BINOPEXP: {
    leftResult = foldTraverse(left);
    rightResult = foldTraverse(right);
    if (leftResult->expressionType() != EXPCOMPREF &&
        rightResult->expressionType() != EXPCOMPREF) {
      return newAST_Expression_Integer((int) pow(getNumericExpressionVal(leftResult), getNumericExpressionVal(rightResult)));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPEXP);
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
      switch (arrayIndex->expressionType()) {
      case EXPBINOP:
      {
        AST_Expression_BinOp binOp = arrayIndex->getAsBinOp();
        AST_Expression newArrayIndex = foldTraverseElement(binOp->left(), binOp->right(), binOp->binopType());
        newIndexes->push_back(newArrayIndex);
      }
        break;
      case EXPCOMPREF:
      {
        AST_Expression_ComponentReference compRefIndex = arrayIndex->getAsComponentReference();
        newIndexes->push_back(instantiateCompRef(compRefIndex));
      }
        break;
      case EXPINTEGER:
        newIndexes->push_back(arrayIndex);
        break;
      case EXPREAL:
        newIndexes->push_back(arrayIndex);
        break;
      default:
        ERROR("InstantiationTraverse::instantiateCompRef: \n"
            "Incorrect array index type %d.\n", arrayIndex->expressionType());
      }
    }
    newArray->append((AST_String)current_element(namesIter), newIndexes);
    namesIter++;
  }
  return newArray;
}

AST_Real InstantiationFold::getNumericExpressionVal(AST_Expression exp) {
  if(exp->expressionType() == EXPINTEGER) {
    AST_Expression_Integer integer = exp->getAsInteger();
    return integer->val();
  } else {
    AST_Expression_Real real = exp->getAsReal();
    return real->val();
  }
}
