
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

#include <util/type_check.h>
#define T(s) tyEnv->lookup(s)

TypeCheck_::TypeCheck_(TypeSymbolTable t, VarSymbolTable v): varEnv(v) , tyEnv(t) {};

bool TypeCheck_::check_equal( Type t1 , Type t2 ){
	if ( *t1 == T("Integer") ) {
		if ( *t2 == T("Integer")  or  *t2 == T("Real") ) return true;
		else return false;
	} else if ( *t1 == T("Real") )  {
		if ( *t2 == T("Integer")  or  *t2 == T("Real") ) return true;
		else return false;
	}else {
		return (*t1 == *t2);
	}
	
}

Type TypeCheck_::check_binop( AST_Expression l , AST_Expression r , BinOpType op  )
{
	Type t1 = check_expression(l);
	Type t2 = check_expression(r);
	switch (op) {
		case BINOPLOWER: 
		case BINOPLOWEREQ: 
		case BINOPGREATER: 
		case BINOPGREATEREQ: 
		case BINOPCOMPNE: 
		case BINOPCOMPEQ: 
			if (check_equal(t1,t2) and !check_equal(t1 , T("Boolean")) ) 	return T("Boolean");
			else throw "Type Error (0)";
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
			if (check_equal(t1,t2) and ( check_equal(t1 , T("Real"))  or check_equal(t1 , T("Integer")) )  )
				return t1;
			else throw "Type Error (1)"	;
		case BINOPAND:
		case BINOPOR:
			if (check_equal(t1,t2) and check_equal(t1 , T("Boolean")) ) return t1;
			throw "Type Error (2)"; 
	}
}

Type TypeCheck_::check_expression(AST_Expression e)
{
	Type ct,t1,t2,t;
	switch (e->expressionType()) {
		case EXPBINOP:
		{
			AST_Expression_BinOp b = e->getAsBinOp();
			return check_binop(b->left() , b->right() , b->binopType());
		}
		case EXPUMINUS: 
		{
			AST_Expression_UMinus b = e->getAsUMinus();
			t = check_expression(b->exp());
			if ( check_equal(t , T("Integer")) or check_equal(t , T("Real")) ) return t;
			throw "Type Error (3)"; 
		}
		
		case EXPOUTPUT :
		{
			AST_Expression_Output b = e->getAsOutput();
			return check_expression(b->getExpressionList()->front() );
		}  
		  
		case EXPIF:
		{   
			AST_Expression_If b = e->getAsIf();
			ct = check_expression(b->condition() );
			t1 = check_expression(b->then() );
			t2 = check_expression(b->else_exp()); // Falta el elseIF
			if ( !check_equal(ct, T("Boolean")) ) throw "Type Error (4)"; 
			if ( !check_equal(t1,t2) ) throw "Type Error (5)"; 
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
			t = check_expression(b->exp());
			if ( !check_equal(t, T("Boolean")) ) throw "Type Error (6)"; 
			return t;
		}
	}
}
