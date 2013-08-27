
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
#include <cstdlib>

#include <parser/parse.h>
#include <util/debug.h>
#include <antialias/remove_alias.h>

using namespace std;

int main(int argc, char ** argv)
{
  RemoveAlias ra;

  TypeSymbolTable tyEnv = newTypeSymbolTable();
  int r; int opt;

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
  AST_Class c;
  if (argv[optind]!=NULL) 
    c=parseClass(argv[optind],&r);
  else
    c=parseClass("",&r);
 
  if (r!=0) 
    return -1;


  if (r!=0)
    return -1;

  MMO_Class mc = newMMO_Class(c,tyEnv);
  ra.removeAliasEquations(mc);
  cout << mc;
  delete mc;
  delete tyEnv;

  return 0;
}
