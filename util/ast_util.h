
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

class AST_Expression_Traverse {
public:
  AST_Expression mapTraverse(AST_Expression);
private:
  virtual AST_Expression mapTraverseElement(AST_Expression) = 0;
};

template<class R>
class AST_Expression_Fold {
public:
  R foldTraverse(AST_Expression e) {
    switch (e->expressionType()) {
      case EXPBINOP:
        {
        AST_Expression_BinOp b = e->getAsBinOp();
        return foldTraverseElement (foldTraverse(b->left()),foldTraverse(b->right()),b->binopType());
        }
      default:
        return foldTraverseElement(e);
    }
  };
private:
  virtual R foldTraverseElement(AST_Expression) = 0;
  virtual R foldTraverseElement(R, R, BinOpType) = 0;

};

class IsConstant: public AST_Expression_Fold<bool> {
private:
  virtual bool foldTraverseElement(AST_Expression);
  virtual bool foldTraverseElement(bool , bool , BinOpType);
};
