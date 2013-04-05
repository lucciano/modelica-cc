
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

#ifndef STORED_DEFINITION
#define STORED_DEFINITION
#include <ast/ast_node.h>
#include <ast/class.h>

class AST_StoredDefinition_: public AST_Node
{
public:
  AST_StoredDefinition_(AST_ClassList ml, AST_String within);
  friend ostream & operator<<(ostream &, const AST_StoredDefinition_ &) ;
  AST_ClassList models() const { return _model_list; }
  AST_String within() const { return _within; }

private:
  AST_ClassList _model_list;
  AST_String _within;
} ;
#endif
