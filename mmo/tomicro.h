
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
#include <mmo/mmo_class.h>
#include <ast/equation.h>
#include <util/type.h>


#ifndef TOMICRO_H
#define TOMICRO_H

#define VAR(v)		newAST_Expression_ComponentReferenceExp(v) 
#define GREATER(l,r) 	newAST_Expression_BinOp(l, r, BINOPGREATER ) 
#define LOWER(l,r) 	newAST_Expression_BinOp(l, r, BINOPLOWER )
#define ADD(l,r) 	newAST_Expression_BinOp(l, r, BINOPADD )
#define MULT(l,r) 	newAST_Expression_BinOp(l, r, BINOPMULT )
#define SUB(l,r) 	newAST_Expression_BinOp(l, r, BINOPSUB )
#define I(n) 		newAST_Expression_Integer(n)
#define PA(e)       newAST_Expression_OutputExpressions(e) 
#define UMENOS(e)   SUB( I(1) , e )


class MMO_ToMicroModelica_ {
public:
	MMO_ToMicroModelica_(MMO_Class * c );
	
	void transform();
	
private:
	MMO_Class * _c;
	AST_String  new_label();
	
	void toMicro_eq_if(AST_Equation_If);
	MMO_Statement toMicro_eq_when (AST_Equation eq);
	MMO_Equation toMicro_eq_equality(AST_Equation_Equality eq); 
	AST_Expression toMicro_exp(AST_Expression);
	 
	AST_Expression whenCondition(AST_Expression e, AST_StatementList ls );
	MMO_Statement make_when(AST_Expression cond , AST_Expression_ComponentReference var);
	
	AST_Expression makeCondition(AST_ExpressionList ls , int n );
	AST_Expression makeEquation(AST_EquationList ls , AST_ExpressionList cond); 
 
	
};


#endif
