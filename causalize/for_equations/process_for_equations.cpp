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
#include <causalize/for_equations/process_for_equations.h>
#include <causalize/for_equations/equation_instantiation_traverse.h>
#include <causalize/for_equations/for_index_iterator.h>

void process_for_equations(MMO_Class mmo_class) {

  MMO_EquationList equations = mmo_class->getEquations();

  MMO_EquationListIterator eqIter, auxiliaryIter;
  AST_ExpressionListIterator expIter;

  auxiliaryIter = equations->begin();
  for(eqIter = auxiliaryIter; eqIter != equations->end(); eqIter = auxiliaryIter) {
    ++auxiliaryIter;
    MMO_Equation eq = current_element(eqIter);
    if (eq->equationType() == EQFOR) {
      AST_Equation_For forEq = (AST_Equation_For) eq->getAsFor();
      AST_ForIndexList forIndexList = forEq->forIndexList();
      // TODO [Moya] for with more than 1 forIndex
      ERROR_UNLESS(forIndexList->size() == 1,
          "process_for_equations:\n"
          "forIndexList with more than 1 forIndex are not supported yet\n");
      AST_ForIndex forIndex = forIndexList->front();
      AST_String variable = forIndex->variable();
      AST_Expression inExp = forIndex->in_exp();
      if (inExp != NULL) {
        ForIndexIterator rangeIter = processInExp(inExp);
        while (rangeIter->hasNext()) {
          AST_Integer index = rangeIter->next();
          AST_EquationList innerEqs = forEq->equationList();
          AST_EquationListIterator innerEqsiter;
          foreach(innerEqsIter, innerEqs) {
            AST_Equation innerEq = current_element(innerEqsIter);
            // TODO [Moya] Ver si esta bien trabajar directamente
            // con el objeto innerEq que "pertencecia" a la
            // for-equation. O si tendriamos que crear un objeto
            // MMO_Equation nuevo.
            instantiate_equation(innerEq, variable, index);
            mmo_class->addEquation(innerEq);
          }
        }
      } else {
        // TODO [Moya] Implicit range
        ERROR("for-equation's index with implicit range not supported yet\n");
      }
      mmo_class->removeEquation(eq); // Remove FOR Equation.
    }
  }

}

ForIndexIterator processInExp(AST_Expression inExp){
  switch (inExp->expressionType()) {
  case EXPRANGE:
    ForIndexIterator iterator = new RangeIterator(inExp->getAsRange());
    return iterator;
    break;
  case EXPBRACE:
    ForIndexIterator iterator = new BraceIterator(inExp->getAsBrace());
    return iterator;
    break;
  default:
    ERROR("process_for_equations:\n"
        "Equation type not supported in forIndex inExp\n");
  }
}

void instantiate_equation(AST_Equation innerEq, AST_String variable, AST_Integer index) {
    switch (innerEq->equationType()) {
    case EQEQUALITY:
      AST_Equation_Equality eqEq = innerEq->getAsEquality();
      EqInstantiationTraverse traverse = new EqInstantiationTraverse(variable, index);
      mapTraverse(eqEq->left());
      mapTraverse(eqEq->right());
      delete traverse;
      break;
    default:
      ERROR("process_for_equations - instantiate_equation:\n"
            "Incorrect equation type or not supported yet.\n");
    }
}

