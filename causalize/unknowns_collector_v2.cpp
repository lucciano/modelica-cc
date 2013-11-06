/*
 * unknowns_collector_v2.cpp
 *
 *  Created on: 28/10/2013
 *      Author: fede
 */

#include <util/debug.h>
#include <causalize/unknowns_collector_v2.h>


UnknownsCollectorV2::UnknownsCollectorV2(MMO_Class mmo_class) {
  _mmo_class = mmo_class;
  _varSymbolTable = mmo_class->getVarSymbolTable();
  _unknowns = newAST_ExpressionList();
}

AST_ExpressionList UnknownsCollectorV2::collectUnknowns() {
  MMO_EquationList eqs = _mmo_class->getEquations();
  MMO_EquationListIterator iter;
  foreach(iter, eqs) {
    MMO_Equation eq = current_element(iter);
    switch(eq->equationType()) {
    case EQEQUALITY: {
      AST_Equation_Equality eqEq = eq->getAsEquality();
      AST_Expression left = eqEq->left();
      AST_Expression right = eqEq->right();
      AST_Expression_Traverse::mapTraverse(left);
      AST_Expression_Traverse::mapTraverse(right);
      break;
    } default:
      ERROR("UnknownsCollectorV2::collectUnknowns(): \n"
          "Only Equality Equations are supported here.\n");
    }
  }
  return _unknowns;
}

AST_Expression UnknownsCollectorV2::mapTraverseElement(AST_Expression exp) {
  switch(exp->expressionType()) {
      case EXPDERIVATIVE: {
        AST_Expression_Derivative der = exp->getAsDerivative();
        AST_ExpressionList arguments = der->arguments();
        ERROR_UNLESS(arguments->size() == 1, "StateVariablesFinder::mapTraverseElement:\n"
            "AST_Expression_Derivative with more than one argument are not supported yet.\n");
        AST_Expression argument = arguments->front();
        switch(argument->expressionType()) {
          case EXPCOMPREF:
          {
            AST_Expression_ComponentReference compRef = argument->getAsComponentReference();
            VarInfo varInfo = getVarInfo(compRef);
            varInfo->setState();
            _unknowns->push_back(der);
          }
            break;
          default:
            ERROR("UnknownsCollectorV2::mapTraverseElement:\n"
                "Incorrect expression type of AST_Expression_Derivative argument\n.");
        }
        break;
      } case EXPCOMPREF: {
        AST_Expression_ComponentReference compRef = exp->getAsComponentReference();
        VarInfo varInfo = getVarInfo(compRef);
        if (varInfo->type()->getType() == TYREAL &&
            !varInfo->builtIn() &&
            !varInfo->isConstant() &&
            !varInfo->isDiscrete() &&
            !varInfo->isParameter() &&
            !varInfo->isState() &&
            !varInfo->isUnknown()) {
          varInfo->setUnknown();
          _unknowns->push_back(compRef);
        }
        break;
      }
    }
    return exp;
}

VarInfo UnknownsCollectorV2::getVarInfo(AST_Expression_ComponentReference compRef) {
  VarInfo varInfo;
  AST_StringList names = compRef->names();
  if (names->size() > 0) {
    // TODO [Moya] Ver como buscar en la tabla de símbolos arreglos con muchos names
    ERROR_UNLESS(names->size() == 1, "StateVariablesFinder::mapTraverseElement:\n"
                "AST_Component_Reference with names list bigger than 1 are not supported yet.\n");
    AST_String name = names->front();
    varInfo = _varSymbolTable->lookup(*name);
  } else {
    varInfo = _varSymbolTable->lookup(compRef->name());
  }
  return varInfo;
}
