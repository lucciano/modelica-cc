
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



#include <ast/ast_builder.h>
#include <mmo/traverse_example.h>
#include <mmo/mmo_class.h>
#include <parser/parse.h>
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <util/type.h>
#include <util/symbol_table.h>
#include <util/type_check.h>
#include <mmo/reduce_equation.h>
#include <mmo/replace_equation.h>
#include <iostream>
using namespace std;


int main(int argc, char ** argv)
{
  int r;
  ReplaceIf rep_sum;
  if (argc<2) {
    cerr << "Usage:\n\tmcc file.mo\n";
    return -1;
  }
  TypeSymbolTable tyEnv = new TypeSymbolTable_;
  
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  
  if (r==0) { // Parsed ok
    AST_Class c = sd->models()->front();
    cerr << c << "---------------------" << endl;
    MMO_Class * d = new MMO_Class(c, tyEnv);

	 
	MMO_Reduce_Equation_ * re = new MMO_Reduce_Equation_(d);  
	 
	cerr << "Reduciendo Ecuaciones" << endl;
	 
    AST_EquationListIterator eqit;
    int a = 0;
    foreach(eqit,d->getEquations()) {
		current(eqit) =  re->simplify( current(eqit) ) ;
	}
	
	foreach(eqit,d->getEquations()) {
		cerr <<  current(eqit)  ;
	}
	
	
	cerr << endl << "Reemplazando Ecuaciones" << endl;
	MMO_Replace_Equation_ *   replace =  new MMO_Replace_Equation_(d);
	try {
		replace->replace();
	} catch (char const * c) {  cerr << c << endl;}
	
	
	foreach(eqit,d->getEquations()) {
		cerr <<  current(eqit)  ;
	}
	
	AST_StatementListIterator stit;
	foreach(stit,d->getStatements()) {
		cerr << current(stit) << endl;
	}
	
	
	//cerr << c << "---------------------" << endl;
	
    cerr << "----------------------" << endl << " Variables: " << endl;
    AST_ComponentListIterator cit;
    foreach(cit,d->getComponents()) {
		cerr << "-> " << current(cit)->name() << ":" <<  d->getVarSymbolTable()->lookup(current(cit)->name())->type() << endl;	
	}
	
	
	
  }
  
  return 0;
}
