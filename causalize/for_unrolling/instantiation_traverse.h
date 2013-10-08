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

#include <ast/ast_types.h>
#include <util/ast_util.h>

class InstantiationTraverse : public AST_Expression_Traverse  {
public:
  AST_Expression instantiate(AST_String variable, AST_Integer index, AST_Expression exp);
private:
  virtual AST_Expression mapTraverseElement(AST_Expression);
  AST_Expression instantiateCompRef(AST_Expression_ComponentReference compRef);
  AST_Expression instantiateArray(AST_Expression_ComponentReference exp);
  AST_Real getNumericExpressionVal(AST_Expression exp);
  AST_Expression evalExp(AST_Expression exp);
  AST_Expression evalBinOp(AST_Expression_BinOp binOpExp);
  AST_String _forIndex_variable;
  AST_Integer _forIndex;
};

