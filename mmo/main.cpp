
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

#include <ast/ast_builder.h>
#include <mmo/traverse_example.h>
#include <mmo/mmo_class.h>
#include <parser/parse.h>
#include <ast/stored_definition.h>
#include <ast/class.h>
#include <util/type.h>

using namespace std;

/*
AST_Expression ReplaceIf(AST_Expression e) {
  cerr << e->expressionType() << endl;	
  switch (e->expressionType()) {
	case EXPOUTPUT :
	{
		AST_Expression_Output b = e->getAsOutput();
        AST_ExpressionList ls = new list < AST_Expression > ();
        ls->push_back(  ReplaceIf ( b->getExpressionList()->front() )   )	;
        return newAST_Expression_OutputExpressions(ls);
	}
	case EXPBINOP:
      {
        AST_Expression_BinOp b = e->getAsBinOp();
        return newAST_Expression_BinOp ( ReplaceIf(b->left()) , ReplaceIf(b->right() ), b->binopType() ) ;
	  }
    case EXPIF: 
      {
        AST_Expression_If i = e->getAsIf();
        ExpressionType cond = i->condition()->expressionType();
        if (cond != EXPCOMPREF)
			throw "Not suported yet!!";
		
		// For now here	
		#define ADD(l,r) 	newAST_Expression_BinOp(l, r, BINOPADD )
		#define MULT(l,r) 	newAST_Expression_BinOp(l, r, BINOPMULT )
		#define SUB(l,r) 	newAST_Expression_BinOp(l, r, BINOPSUB )
		#define I(n) 		newAST_Expression_Integer(n)
		#define PA(e)       newAST_Expression_OutputExpressions(e) 
			
		AST_ExpressionList ls = new list < AST_Expression > ();
		ls->push_back(SUB( I(1) , i->condition() ))	;
			
		AST_Expression l = MULT(i->condition() , ReplaceIf(i->then()) );
        AST_Expression r = MULT(   PA ( ls ) ,   ReplaceIf(i->else_exp()) );
        
        
        return ADD(l,r);
      }
  }
  // No modification
  return e;
}
*/


int main(int argc, char ** argv)
{
  int r;
  ReplaceIf rep_sum;
  if (argc<2) {
    cerr << "Usage:\n\tmcc file.mo\n";
    return -1;
  }
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  if (r==0) { // Parsed ok
    AST_Class c = sd->models()->front();
    cerr << c << "---------------------" << endl;
    
    MMO_Class * d = new MMO_Class(c);

	//cerr << " Primera aproximacion  " << endl;
    AST_EquationListIterator eqit;
    foreach(eqit,d->getEquations()) {
		AST_Equation_Equality r = current(eqit)->getAsEquality();
		AST_Equation _r = newAST_Equation_Equality( r->left() , rep_sum.mapTraverse(r->right()) );
		 
		current(eqit) =  _r ;
		 //cerr << rep_sum.mapTraverse(r->right()) << endl;
	}

	
	//cerr << c << "---------------------" << endl;
	
    cerr << "----------------------" << endl << " Variables: " << endl;
    AST_ComponentListIterator cit;
    foreach(cit,d->getComponents()) {
		cerr << current(cit) << endl;	
	}
	
	
	cerr << "----------------------" << endl << " Pruebas con Tipos: " << endl;
	Type_String s = new Type_String_;
	Type_Real i = new Type_Real_;
	Type_Array  rr = new Type_Array_(i);
	Type_Array  rs = new Type_Array_(s);
	
	Type_Array  ar = new Type_Array_(rr);
	
	cerr << rr << endl;
	cerr << rs << endl;
	if (*rs == *rr) cerr << "Yes!" << endl;
	else cerr << "No!" << endl;
	
  }
  
  return 0;
}
