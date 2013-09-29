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

class ForIndexIterator {
public:
  virtual bool hasNext() = 0;
  virtual AST_Integer next() = 0;
};

class RangeIterator : ForIndexIterator {
public:
  RangeIterator(AST_Expression_Range range);
  bool hasNext();
  AST_Integer next();
private:
  void initReal(AST_ExpressionListIterator iter);
  void initInteger(AST_ExpressionListIterator iter);
  AST_ExpressionList _rangeElements;
  AST_Real _rangeBegin;
  AST_Real _rangeStep;
  AST_Real _rangeEnd;
  AST_Real _current;
};

class BraceIterator : ForIndexIterator {
public:
  BraceIterator(AST_Expression_Brace braceExp);
  bool hasNext();
  AST_Integer next();
private:
  AST_ExpressionList _braceExpElements;
  AST_ExpressionListIterator _braceExpElementsIter;
};
