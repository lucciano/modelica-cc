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
	int i; int symbolTableSize = symbolTable->count();
	for (i = 0; i<symbolTableSize; i++) {
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
    switch (arrayDimension->expressionType()){
    case EXPCOMPREF:
      return getCompRefVal(arrayDimension->getAsComponentReference(), symbolTable);
    case EXPINTEGER:
      return arrayDimension->getAsInteger()->val();
    case EXPREAL:
      return arrayDimension->getAsReal()->val();
    default:
      ERROR("Incorrect array dimension type or not supported yet");
    }
}

int UnknownsCollector::getCompRefVal(AST_Expression_ComponentReference compRef, VarSymbolTable symbolTable) {
  VarInfo vInfo = symbolTable->lookup(compRef->name());
  ERROR_UNLESS(vInfo->isConstant() || vInfo->isParameter(),
      "RangeIterator::getCompRefVal\n"
      "AST_Component_Reference in AST_Expression_Range must be constant or parameter.\n");
  AST_Modification mod = vInfo->modification();
  switch(mod->modificationType()) {
  case MODEQUAL: {
    AST_Modification_Equal equal = mod->getAsEqual();
    AST_Expression exp = equal->exp();
    switch (exp->expressionType()) {
    case EXPINTEGER:
      return exp->getAsInteger()->val();
    case EXPREAL:
      return (int) exp->getAsReal()->val();
    default:
      ERROR("RangeIterator::getCompRefVal\n"
          "For now only literals are supported as AST_Modification_Equal expression\n");
    }
    break;
  } default:
    ERROR("RangeIterator::getVal\n"
        "Incorrect AST_Modification type or not supported yet.\n");
  }
}

AST_Expression UnknownsCollector::buildArray(AST_String name, int index) {
  AST_Expression_ComponentReference compRef = (AST_Expression_ComponentReference) newAST_Expression_ComponentReference();
  AST_ExpressionList indexList = newAST_ExpressionList();
  indexList->push_back(newAST_Expression_Integer(index));
  compRef->append(name, indexList);
  return compRef;
}
