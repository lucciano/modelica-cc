
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


#ifndef REDUCEEQUATIO_H
#define REDUCEEQUATIO_H


class MMO_Reduce_Equation_ {
public:
	MMO_Reduce_Equation_(MMO_Class * c );
	
	void simplifyAll();
	
private:
	MMO_Class * _c;
	AST_Expression simplify_real(AST_Expression e);
	AST_Expression simplify_bool(AST_Expression e);
	AST_Expression simplify_condition(AST_Expression e);
	void reduce_eq_if(AST_Equation_If);
	bool simpleExpresion(AST_Expression e);
	AST_Equation  simplify( AST_Equation e );
	AST_String  new_label();
};


#endif
