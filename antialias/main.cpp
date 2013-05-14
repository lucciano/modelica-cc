
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
#include <util/symbol_table.h>
#include <mmo/mmo_class.h>
#include <antialias/remove_alias.h>

using namespace std;

int main(int argc, char ** argv)
{
  int r;
  if (argc<2) {
    cerr << "Usage:\nantialias file.mo\n";
    return -1;
  }
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  if (r!=0) 
    return -1;

  TypeSymbolTable tyEnv = new TypeSymbolTable_;
  MMO_Class *c = new MMO_Class(sd->models()->front(), tyEnv);
  RemoveAlias ra(c);
  ra.removeAliasEquations();
	MMO_EquationList eqs = c->getEquations();
	MMO_EquationListIterator eqit;
  if (eqs != NULL) {
    foreach(eqit, eqs) 
      cerr << current(eqit);
  }
  cerr << sd->models()->front();

  return 0;
}
