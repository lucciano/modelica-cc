
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

#include <cstdlib>
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
#include <causalize/debug.h>


using namespace std;

int main(int argc, char ** argv)
{

  int r;
  int opt;

  if (argc<2) {
    cerr << "Usage:\ncausalize [-d] file.mo\n";
    return -1;
  }

  while ((opt = getopt(argc, argv, "d:")) != -1) {
    switch (opt) {
     case 'd':
       if (optarg != NULL && isDebugParam(optarg)) {
         debugInit(optarg);
       } else {
         ERROR("command-line option d has no arguments\n");
       }
       break;
    }
  }

  AST_StoredDefinition sd = parseFile(argv[optind],&r);
  if (r!=0) 
    return -1;

  TypeSymbolTable tyEnv = newTypeSymbolTable();
  MMO_Class c = newMMO_Class(sd->models()->front(), tyEnv);

  MMO_EquationListIterator iter;
  MMO_EquationList acausalEquations = c->getEquations();

  DEBUG('c', "Acausal Equations:\n");
  cout << c;
  foreach(iter, acausalEquations) {
    DEBUG('c', "%s", current_element(iter)->print().c_str());
  }

  StateVariablesFinder *stateFinder = new StateVariablesFinder(c);
  stateFinder->findStateVariables();
  DiscreteVariablesFinder *discreteVarFinder = new DiscreteVariablesFinder(c);
  discreteVarFinder->findDiscreteVariables();
  UnknownsCollector *collector = new UnknownsCollector(c);
  AST_ExpressionList unknowns = collector->collectUnknowns();
  MMO_EquationList causalEqs = newMMO_EquationList();
  causalize(acausalEquations, unknowns, causalEqs);

  DEBUG('c', "Causalized Equations:\n");
  c->getEquations()->clear();
  foreach(iter, causalEqs) {
    DEBUG('c', "%s", current_element(iter)->print().c_str());
    AST_ListAppend(c->getEquations(),current_element(iter));
  }
  cout << c;
  return 0;
}
