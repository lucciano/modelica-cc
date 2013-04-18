
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
#include <util/symbol_table.h>

using namespace std;



Type checkType_Expresion(AST_Expression e, VarSymbolTable varEnv , TypeSymbolTable * tyEnv )
{
	#define T(s) tyEnv->lookup(s)
	Type ct,t1,t2,t;
	switch (e->expressionType()) {
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();	
			t1 = checkType_Expresion(b->left() ,varEnv,tyEnv);
			t2 = checkType_Expresion(b->right() ,varEnv,tyEnv);
			
			switch (b->binopType()) {
				case BINOPLOWER: 
				case BINOPLOWEREQ: 
				case BINOPGREATER: 
				case BINOPGREATEREQ: 
				case BINOPCOMPNE: 
				case BINOPCOMPEQ: 
					cerr << t1 << "  " << t2 << endl; 
					if (t1 == t2 ) return T("Boolean");
					else throw "Type Error (1)"; /* Faltaria chequear que no sean bool */	
					break;
				case BINOPDIV: 
				case BINOPELDIV:
				case BINOPMULT:
				case BINOPELMULT: 
				case BINOPADD: 
				case BINOPELADD:
				case BINOPSUB: 
				case BINOPELSUB: 
				case BINOPEXP: 
				case BINOPELEXP:
					if ( *t1 != t2)  throw "Type Error (2)"; 
					if ( *t1 != T("Real") and *t1 != T("Integer")) throw "Type Error (3)"; 
					return t1;
					break;
				case BINOPAND:
				case BINOPOR:
					if ( *t1 != t2 or *t1 != T("Boolean") ) throw "Type Error (4)"; 
					return t1;
					break;
			}
			
		}
		case EXPUMINUS: 
		{
			AST_Expression_UMinus b = e->getAsUMinus();
			t = checkType_Expresion(b->exp() ,varEnv,tyEnv);
			if ( *t != T("Real")  and *t != T("Integer") ) throw "Type Error (5)"; 
			return t;
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			return checkType_Expresion(b->getExpressionList()->front() , varEnv , tyEnv );
		}  
		  
		case EXPIF:
		{   
			AST_Expression_If b = e->getAsIf();
			ct = checkType_Expresion(b->condition() ,varEnv,tyEnv);
			t1 = checkType_Expresion(b->then() ,varEnv,tyEnv);
			t2 = checkType_Expresion(b->else_exp() ,varEnv,tyEnv); // Falta el elseIF
			if ( *ct != T("Boolean") ) throw "Type Error (6)"; 
			if ( *t1 != t2) throw "Type Error (7)"; 
			return t1;
			
		}
		
		case EXPCOMPREF: 
		{
			AST_Expression_ComponentReference b = e->getAsComponentRef();
			VarInfo * tt = varEnv->lookup(b->name());
			if (tt == NULL)  throw "Variable no existe"; 
			return tt->type();
			break;
		}
		case EXPDERIVATIVE: 
			return T("Real");
		case EXPBOOLEAN:
			return T("Boolean");
		case EXPSTRING: 
			return T("String");
		case EXPREAL: 
			return T("Real");
		case EXPINTEGER:
			return T("Integer");
		case EXPBOOLEANNOT:	
		{
			AST_Expression_BooleanNot b = e->getAsBooleanNot();
			t = checkType_Expresion(b->exp() ,varEnv,tyEnv);
			if ( *t != T("Boolean") ) throw "Type Error"; 
			return t;
		}
	}
}




int main(int argc, char ** argv)
{
  int r;
  ReplaceIf rep_sum;
  if (argc<2) {
    cerr << "Usage:\n\tmcc file.mo\n";
    return -1;
  }
  TypeSymbolTable tyEnv;
  AST_StoredDefinition sd = parseFile(argv[1],&r);
  if (r==0) { // Parsed ok
    AST_Class c = sd->models()->front();
    cerr << c << "---------------------" << endl;
    
    MMO_Class * d = new MMO_Class(c);

	//cerr << " Primera aproximacion  " << endl;
    AST_EquationListIterator eqit;
    foreach(eqit,d->getEquations()) {
		AST_Equation_Equality r = current(eqit)->getAsEquality();
		//AST_Equation _r = newAST_Equation_Equality( r->left() , rep_sum.mapTraverse(r->right()) );
		//current(eqit) =  _r ;
		try {
		 cerr << "Tipo:" <<  checkType_Expresion(r->right(),d->getVarSymbolTable() , &tyEnv  )  << endl;
		} catch(char const * s) {cerr << s << endl;}
	}

	
	//cerr << c << "---------------------" << endl;
	
    cerr << "----------------------" << endl << " Variables: " << endl;
    AST_ComponentListIterator cit;
    foreach(cit,d->getComponents()) {
		cerr << "-> " << current(cit)->name() << ":" <<  d->getVarSymbolTable()->lookup(current(cit)->name())->type() << endl;	
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
	
	VarSymbolTable varEnv =  new VarSymbolTable_;
	
	varEnv->insert("Hola" , NULL);
	cerr << "Desde la tabla! "<< tyEnv.lookup("String") << endl;
	
	//string str;
	//cin >> str;
	//cerr << "Desde la clase: " <<  d->getVarSymbolTable()->lookup(str)->type() << endl;
	
	
  }
  
  return 0;
}
