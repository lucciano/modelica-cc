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

#include <util/debug.h>
#include <util/symbol_table.h>
#include <util/ast_util.h>

#include <causalize/for_unrolling/process_for_equations.h>
#include <causalize/for_unrolling/for_index_iterator.h>

ForIndexIterator *processInExp(AST_Expression inExp, VarSymbolTable symbolTable){
  ForIndexIterator *iterator;
  switch (inExp->expressionType()) {
  case EXPRANGE:
    iterator = new RangeIterator(inExp->getAsRange(), symbolTable);
    return iterator;
    break;
  case EXPBRACE:
    iterator = new BraceIterator(inExp->getAsBrace());
    return iterator;
    break;
  default:
    ERROR("process_for_equations:\n"
        "Equation type not supported in forIndex inExp\n");
  }
}

MMO_Equation instantiate_equation(AST_Equation innerEq, AST_String variable, AST_Integer index, VarSymbolTable symbolTable) {
  EvalExp evaluator(symbolTable);
  switch (innerEq->equationType()) {
    case EQEQUALITY: {
      AST_Equation_Equality eqEq = innerEq->getAsEquality();
      AST_Expression_ComponentReference compRef = (AST_Expression_ComponentReference) newAST_Expression_ComponentReferenceExp(variable);
      AST_Expression_Integer integerExp = (AST_Expression_Integer) newAST_Expression_Integer(index);
      AST_Expression left = evaluator.eval(compRef, integerExp, eqEq->left());
      AST_Expression right = evaluator.eval(compRef, integerExp, eqEq->right());
      return newAST_Equation_Equality(left, right);
    } default:
      ERROR("process_for_equations - instantiate_equation:\n"
            "Incorrect equation type or not supported yet.\n");
    }
}

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
      ERROR_UNLESS(forIndexList->size() == 1,
          "process_for_equations:\n"
          "forIndexList with more than 1 forIndex are not supported yet\n");
      AST_ForIndex forIndex = forIndexList->front();
      AST_String variable = forIndex->variable();
      AST_Expression inExp = forIndex->in_exp();
      if (inExp != NULL) {
        ForIndexIterator *forIndexIter = processInExp(inExp, mmo_class->getVarSymbolTable());
        while (forIndexIter->hasNext()) {
          AST_Real index = forIndexIter->next();
          AST_EquationList innerEqs = forEq->equationList();
          AST_EquationListIterator innerEqsIter;
          foreach(innerEqsIter, innerEqs) {
            AST_Equation innerEq = current_element(innerEqsIter);
            MMO_Equation newEq = instantiate_equation(innerEq, variable, index, mmo_class->getVarSymbolTable());
            mmo_class->addEquation(newEq);
          }
        }
      } else {
        ERROR("for-equation's index with implicit range not supported yet\n");
      }
      mmo_class->removeEquation(eq); // Remove FOR Equation.
    }
  }

}





