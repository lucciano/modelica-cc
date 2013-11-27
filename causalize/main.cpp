
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
#include <stdio.h>
#include <iostream>

#include <parser/parse.h>
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <ast/ast_types.h>
#include <causalize/causalization_strategy.h>
#include <mmo/mmo_class.h>
#include <util/debug.h>
#include <util/symbol_table.h>


using namespace std;

void printParameter(FILE *params, VarName name, VarInfo var) {
  if (var->isParameter()) {
    fprintf(params,"#ifdef QSS_SOLVER\n");
    fprintf(params,"extern double %s;\n",name.c_str());
    fprintf(params,"#else\n");
    if (var->modification()!=NULL && var->modification()->modificationType()==MODEQUAL) {
      AST_Modification_Equal me = var->modification()->getAsEqual();
      /* TODO: This should evaluate instead of just printing the exp */
      fprintf(params,"double %s=%s;\n",name.c_str(),me->exp()->print().c_str());
    } else if (var->modification()!=NULL && var->modification()->modificationType()==MODCLASS) {
      AST_Modification_Class mc = var->modification()->getAsClass();
      /* TODO: This should evaluate instead of just printing the exp */
      fprintf(params,"double %s=%s;\n",name.c_str(),mc->exp()->print().c_str());
    } else {
      fprintf(params,"double %s;\n",name.c_str());
    }
    fprintf(params,"#endif\n");
  }
}
void printParameters(MMO_Class c) {
  string file_name= toStr(c->name());
  VarSymbolTable varEnv = c->getVarSymbolTable();
  file_name.append("_parameters.h");
  FILE *params=fopen(file_name.c_str(),"w");
  if (c->parameters()==NULL) {
    for(int i=0;i<varEnv->count();i++) {
      VarInfo var = varEnv->val(i);
      printParameter(params,varEnv->key(i),var);
   }
  } else {
    AST_StringListIterator it;
    foreach(it,c->parameters()) {
      VarInfo var = varEnv->lookup(toStr(current_element(it)));
      printParameter(params,toStr(current_element(it)),var);
   }
  }
  fclose(params);
 

}
int main(int argc, char ** argv)
{

  int r;
  int opt;
  bool for_qss;

  /*if (argc<2) {
    cerr << "Usage:\ncausalize [-d] file.mo\n";
    return -1;
  }*/

  while ((opt = getopt(argc, argv, "d:q")) != -1) {
    switch (opt) {
     case 'd':
       if (optarg != NULL && isDebugParam(optarg)) {
         debugInit(optarg);
       } else {
         ERROR("command-line option d has no arguments\n");
       }
       break;
     case 'q':
       for_qss=true;
       break;
    }
  }

  AST_Class ast_c;
  if (argv[optind]!=NULL) 
    ast_c=parseClass(argv[optind],&r);
  else
    ast_c=parseClass("",&r);
 
  if (r!=0) 
    return -1;

  TypeSymbolTable tyEnv = newTypeSymbolTable();
  MMO_Class c = newMMO_Class(ast_c, tyEnv);

  MMO_EquationListIterator iter;
  MMO_EquationList acausalEquations = c->getEquations();

  DEBUG('c', "Acausal Equations:\n");
  foreach(iter, acausalEquations) {
    DEBUG('c', "%s", current_element(iter)->print().c_str());
  }

  CausalizationStrategy *cStrategy = new CausalizationStrategy(c);
  AST_ClassList cl = newAST_ClassList();
  cStrategy->causalize(c->name(), cl);

  DEBUG('c', "Causalized Equations:\n");
  MMO_EquationList causalEqs = c->getEquations();
  MMO_EquationListIterator causalEqsIter;
  foreach(causalEqsIter, causalEqs) {
    DEBUG('c', "%s", current_element(causalEqsIter)->print().c_str());
  }
  c->setfsolve(cl);

  if (for_qss) {
    c->cleanComments();
    c->sortParameters();
  }
  /* Dump parameters file */
  cout << c;
  printParameters(c);
  return 0;
}
