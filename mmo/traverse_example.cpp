
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
#include <ast/expression.h>
#include <mmo/traverse_example.h>

AST_Expression ReplaceSum::mapTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
    case EXPBINOP:
      {
        AST_Expression_BinOp b = e->getAsBinOp();
        switch (b->binopType()) {
          case BINOPADD:
            return newAST_Expression_BinOp(b->left(),b->right(),BINOPMULT);
        }
      }
    case EXPIF: 
      {
        AST_Expression_If i = e->getAsIf();
        return newAST_Expression_BinOp(newAST_Expression_Integer(3),newAST_Expression_Integer(4), BINOPMULT);
      }
  }
  // No modification
  return e;
}
