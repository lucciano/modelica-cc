
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


#ifndef REPLACE_H
#define REPLACE_H

#define ADD(l,r) 	newAST_Expression_BinOp(l, r, BINOPADD )
#define MULT(l,r) 	newAST_Expression_BinOp(l, r, BINOPMULT )
#define SUB(l,r) 	newAST_Expression_BinOp(l, r, BINOPSUB )
#define I(n) 		newAST_Expression_Integer(n)
#define PA(e)       newAST_Expression_OutputExpressions(e) 



class MMO_Replace_Equation_ {
public:
	MMO_Replace_Equation_(MMO_Class * c );
	void replace();
	
	
private:
	MMO_Class * _c;
	AST_Expression reduce_real(AST_Expression e);
	AST_Expression reduce_bool(AST_Expression_ComponentReference v , AST_Expression e);
	AST_Expression reduce_condition ( AST_Expression , AST_Expression , AST_Expression  );
	MMO_Statement make_when(AST_Expression cond , AST_Expression_ComponentReference var);
	AST_Expression generate_condition( AST_Expression c );
};


#endif
