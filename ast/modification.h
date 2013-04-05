
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

#ifndef AST_MODIFICAITON
#define AST_MODIFICAITON
#include <ast/ast_node.h>
#include <ast/ast_builder.h>

class AST_Argument_: public AST_Node {

};

class AST_ArgumentModification_: public AST_Argument {
};

class AST_ArgumentRedeclaration_: public AST_Argument {
};

#endif

