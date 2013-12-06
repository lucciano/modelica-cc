/*
 * collect_unknowns.cpp
 *
 *  Created on: 29/04/2013
 *      Author: fede
 */

#include <ast/ast_builder.h>
#include <causalize/unknowns_collector.h>
#include <util/symbol_table.h>
#include <util/type.h>
#include <util/debug.h>
#include <iostream>

UnknownsCollector::UnknownsCollector(MMO_Class c) {
	_c = c;
	_unknowns = newAST_ExpressionList();
	_finder = new StateVariablesFinder(c);

}

UnknownsCollector::~UnknownsCollector() {
}

AST_ExpressionList UnknownsCollector::collectUnknowns() {
  _finder->findStateVariables();
  VarSymbolTable symbolTable = _c->getVarSymbolTable();
	for (int i = 0; i<symbolTable->count(); i++) {
		VarInfo varInfo = symbolTable->varInfo(i);
		if (!varInfo->builtIn()
		    && !varInfo->isConstant()
				&& !varInfo->isDiscrete()
				&& !varInfo->isParameter()) {
		  Type type = varInfo->type();
		  switch (type->getType()) {
        case TYREAL: {
          AST_Expression_ComponentReference compRef = (AST_Expression_ComponentReference) newAST_Expression_ComponentReferenceExp(newAST_String((string) symbolTable->varName(i)));
          if (varInfo->isState()) {
            AST_ExpressionList expList = newAST_ExpressionList(compRef);
            _unknowns->push_back(newAST_Expression_Derivative(expList));
          } else {
            _unknowns->push_back(compRef);
            varInfo->setUnknown();
          }
          break;
        } case TYARRAY: {
          Type_Array arrayType = type->getAsArray();
          Type elementsType = arrayType->arrayOf();
          if (elementsType->getType() == TYARRAY) {
            ERROR("Arrays of arrays are not supported yet\n");
          } else if (elementsType->getType() == TYREAL) {
            int dimension = getDimension(arrayType->dimension(), symbolTable);
            AST_String name = newAST_String((string) symbolTable->varName(i));
            for (int j=1; j<=dimension; j++) {
              if (varInfo->isState()) {
                AST_ExpressionList expList = newAST_ExpressionList(buildArray(name, j));
                _unknowns->push_back(newAST_Expression_Derivative(expList));
              } else {
                _unknowns->push_back(buildArray(name, j));
                varInfo->setUnknown();
              }
            }
          }
        }
		  }
	  }
  }
	return _unknowns;
}

int UnknownsCollector::getDimension(AST_Expression arrayDimension, VarSymbolTable symbolTable) {
  EvalExp evaluator(symbolTable);
  AST_Expression result = evaluator.eval(arrayDimension);
  ERROR_UNLESS(result->expressionType() == EXPINTEGER, "UnknownsCollector::getDimension\n"
      "Array dimension evaluation must return an integer value\n");
  return result->getAsInteger()->val();
}

AST_Expression UnknownsCollector::buildArray(AST_String name, int index) {
  AST_Expression_ComponentReference compRef = (AST_Expression_ComponentReference) newAST_Expression_ComponentReference();
  AST_ExpressionList indexList = newAST_ExpressionList();
  indexList->push_back(newAST_Expression_Integer(index));
  compRef->append(name, indexList);
  return compRef;
}
