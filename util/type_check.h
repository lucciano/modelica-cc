
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

#include <string>
#include <list>
#include <ast/ast_node.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>
#include <util/type.h>
#include <util/symbol_table.h>

#ifndef TYPECHECK_H
#define TYPECHECK_H


class TypeCheck_ {
public:
	TypeCheck_ ( TypeSymbolTable , VarSymbolTable );
	Type check_expression(AST_Expression t);
	
private:
	TypeSymbolTable tyEnv;
	VarSymbolTable varEnv;
	Type check_binop( AST_Expression l , AST_Expression r , BinOpType op  );
	
	/* Mira si dos tipos son iguales (Real == Integer === true) */
	bool check_equal( Type t1 , Type t2 );
};



#endif
