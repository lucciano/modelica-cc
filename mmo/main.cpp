
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

using namespace std;

int main(int argc, char ** argv)
{
  int r;
  ReplaceSum rep_sum;
  AST_Expression e = newAST_Expression_BinOp(newAST_Expression_Integer(3),
      newAST_Expression_If(newAST_Expression_Boolean(true),newAST_Expression_Integer(31),newAST_ExpressionList(),newAST_Expression_Integer(3)),
      BINOPADD);
  cerr << rep_sum.mapTraverse(e) << endl;
  return 0;
}
