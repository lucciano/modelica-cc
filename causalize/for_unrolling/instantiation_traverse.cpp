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
#include <causalize/for_unrolling/instantiation_traverse.h>

AST_Expression InstantiationTraverse::instantiate(AST_String variable, AST_Integer index, AST_Expression exp) {
  _forIndex_variable = variable;
  _forIndex = index;
  return mapTraverse(exp);
}

AST_Expression InstantiationTraverse::mapTraverseElement(AST_Expression exp) {
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

AST_Expression InstantiationTraverse::instantiateCompRef(AST_Expression_ComponentReference compRef) {
  if (compRef->name().compare(*_forIndex_variable) == 0) {
    return newAST_Expression_Integer(_forIndex);
  } else {
    return compRef;
  }
}

// The component reference is an array of the form
// a[i,j].b[x].c[y] (indexesList is ( (i,j) , (x), (y) ).
AST_Expression InstantiationTraverse::instantiateArray(AST_Expression_ComponentReference array) {
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
        AST_Expression newArrayIndex = evalBinOp(arrayIndex->getAsBinOp());
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

AST_Expression InstantiationTraverse::evalExp(AST_Expression exp) {
  switch (exp->expressionType()) {
  case EXPBINOP:
    return evalBinOp(exp->getAsBinOp());
  case EXPREAL:
    return exp;
  case EXPINTEGER:
    return exp;
  case EXPSTRING:{
    AST_Expression_String sExp = exp->getAsString();
    if (sExp->print().compare(*_forIndex_variable) == 0) {
      return newAST_Expression_Integer(_forIndex);
    } else {
      return sExp;
    }
  } default:
    ERROR("process_for_equations.cpp - evalExp:\n"
        "Incorrect expression type.\n");
  }
}

AST_Expression InstantiationTraverse::evalBinOp(AST_Expression_BinOp binOpExp) {
  AST_Expression leftResult;
  AST_Expression rightResult;
  switch(binOpExp->binopType()){
  case BINOPADD: {
    leftResult = evalExp(binOpExp->left());
    rightResult = evalExp(binOpExp->right());
    if (leftResult->expressionType() != EXPSTRING &&
        rightResult->expressionType() != EXPSTRING) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) + getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPADD);
    }
  } case BINOPSUB: {
    leftResult = evalExp(binOpExp->left());
    rightResult = evalExp(binOpExp->right());
    if (leftResult->expressionType() != EXPSTRING &&
        rightResult->expressionType() != EXPSTRING) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) - getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPSUB);
    }
  } case BINOPMULT: {
    leftResult = evalExp(binOpExp->left());
    rightResult = evalExp(binOpExp->right());
    if (leftResult->expressionType() != EXPSTRING &&
        rightResult->expressionType() != EXPSTRING) {
      return newAST_Expression_Integer(getNumericExpressionVal(leftResult) * getNumericExpressionVal(rightResult));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPMULT);
    }
  } case BINOPDIV: {
    leftResult = evalExp(binOpExp->left());
    rightResult = evalExp(binOpExp->right());
    ERROR_UNLESS(rightResult != 0, "process_for_equations - evalExp:\n"
            "Division by zero.\n");
    if (leftResult->expressionType() != EXPSTRING &&
        rightResult->expressionType() != EXPSTRING) {
      return newAST_Expression_Integer((int)(getNumericExpressionVal(leftResult) / getNumericExpressionVal(rightResult)));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPDIV);
    }
  } case BINOPEXP: {
    leftResult = evalExp(binOpExp->left());
    rightResult = evalExp(binOpExp->right());
    if (leftResult->expressionType() != EXPSTRING &&
        rightResult->expressionType() != EXPSTRING) {
      return newAST_Expression_Integer((int) pow(getNumericExpressionVal(leftResult), getNumericExpressionVal(rightResult)));
    } else {
      return newAST_Expression_BinOp(leftResult, rightResult, BINOPEXP);
    }
  } default:
    ERROR("process_for_equations.cpp - evalBinOp:\n"
        "Incorrect Binary operation type.\n");
  }
}


AST_Real InstantiationTraverse::getNumericExpressionVal(AST_Expression exp) {
  if(exp->expressionType() == EXPINTEGER) {
    AST_Expression_Integer integer = exp->getAsInteger();
    return integer->val();
  } else {
    AST_Expression_Real real = exp->getAsReal();
    return real->val();
  }
}