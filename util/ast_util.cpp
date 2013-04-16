
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

#include <util/ast_util.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>

AST_Expression AST_Expression_Traverse::mapTraverse(AST_Expression e) { 
  AST_Expression e2 = mapTraverseElement(e);
  switch (e2->expressionType()) {
    case EXPBINOP:
      {
        AST_Expression_BinOp b = e2->getAsBinOp();
        return newAST_Expression_BinOp(mapTraverseElement(b->left()),mapTraverseElement(b->right()), b->binopType());
      }
    case EXPUMINUS: 
      {
        AST_Expression_UMinus m = e2->getAsUMinus();
        return newAST_Expression_UnaryMinus(mapTraverseElement(m->exp()));
      }
    case EXPIF:
      {
        AST_Expression_If i = e2->getAsIf();
        return newAST_Expression_If(mapTraverseElement(i->condition()), mapTraverseElement(i->then()), i->elseif_list(), mapTraverseElement(i->else_exp()));
      }
  }
  return e2;
}