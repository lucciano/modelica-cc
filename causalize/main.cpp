
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

#include <iostream>

#include <parser/parse.h>
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <ast/ast_types.h>
#include <util/symbol_table.h>
#include <mmo/mmo_class.h>
#include <causalize/state_variables_finder.h>
#include <causalize/discrete_variables_finder.h>
#include <causalize/unknowns_collector.h>
#include <causalize/causalization_strategy.h>

using namespace std;

int main(int argc, char ** argv)
{
  int r;
  if (argc<2) {
    cerr << "Usage:\ncausalize file.mo\n";
    return -1;
  }
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  if (r!=0) 
    return -1;

  TypeSymbolTable tyEnv = newTypeSymbolTable();
  MMO_Class c = newMMO_Class(sd->models()->front(), tyEnv);

  MMO_EquationListIterator iter;
  MMO_EquationList acausalEquations = c->getEquations();

  cout << "Acausal Equations: " << endl;
  foreach(iter, acausalEquations) {
    cout << current_element(iter) << endl;
  }

  StateVariablesFinder *stateFinder = new StateVariablesFinder(c);
  stateFinder->findStateVariables();
  DiscreteVariablesFinder *discreteVarFinder = new DiscreteVariablesFinder(c);
  discreteVarFinder->findDiscreteVariables();
  UnknownsCollector *collector = new UnknownsCollector(c);
  AST_ExpressionList unknowns = collector->collectUnknowns();
  CausalizationStrategy *strategy = new CausalizationStrategy(c->getEquations(), unknowns);
  MMO_EquationList causalizedEquations = strategy->causalize();

  cout << "Causalized Equations: " << endl;
  foreach(iter, causalizedEquations) {
    cout << current_element(iter) << endl;
  }

  return 0;
}
