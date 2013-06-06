/*
 * find_discrete_var.cpp
 *
 *  Created on: 16/05/2013
 *      Author: fede
 */

#include <causalize/discrete_variables_finder.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <assert.h>


DiscreteVariablesFinder::DiscreteVariablesFinder(MMO_Class c) {
  _c = c;
  _varSymbolTable = c->getVarSymbolTable();
}

DiscreteVariablesFinder::~DiscreteVariablesFinder() {
  // TODO Auto-generated destructor stub
}

void DiscreteVariablesFinder::findDiscreteVariables() {
  MMO_EquationList eqs = _c->getEquations();
  MMO_EquationListIterator eqIter;
    if (eqs != NULL) {
      foreach(eqIter, eqs) {
        MMO_Equation eq = current_element(eqIter);
        if(eq->equationType() == EQWHEN) {
          AST_Equation_When eqWhen = eq->getAsWhen();
          findDiscreteVariablesInEquations(eqWhen->equationList());
          AST_Equation_ElseList elseWhenList = eqWhen->equationElseWhen();
          if(elseWhenList != NULL) {
            AST_Equation_ElseListIterator elseWhenListIter;
            foreach(elseWhenListIter, elseWhenList) {
              AST_Equation_Else eqElse = current_element(elseWhenListIter);
              findDiscreteVariablesInEquations(eqElse->equations());
            }
          }
        }
      }
    }
}

void DiscreteVariablesFinder::findDiscreteVariablesInEquations(MMO_EquationList eqs) {
  assert(eqs != NULL);
  MMO_EquationListIterator eqIter;
  foreach(eqIter, eqs) {
    MMO_Equation eq = current_element(eqIter);
    switch(eq->equationType()) {
      case EQEQUALITY:
      {
        AST_Equation_Equality eqEq =  eq->getAsEquality();
        AST_Expression exp = eqEq->left();
        switch (exp->expressionType()){
          case EXPCOMPREF:
          {
            AST_Expression_ComponentReference compRef = exp->getAsComponentRef();
            VarInfo varInfo = _varSymbolTable->lookup(compRef->name());
            if (varInfo != NULL) {
              varInfo->setDiscrete();
            } else {
              // TODO [Moya]
            }
          }
            break;
          case EXPOUTPUT:
            // TODO [Moya]
            break;
          default:
            ;// TODO [Moya] ver como es el manejo de errores y/o excepciones.
        }
      }
        break;
      case EQFOR:
        // TODO [Moya]
        break;
      case EQIF:
        // TODO [Moya]
        break;
      default:
        ;// TODO [Moya] ver como es el manejo de errores y/o excepciones.
    }
  }
}
