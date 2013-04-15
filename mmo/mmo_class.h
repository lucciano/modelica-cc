
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
#include <ast/class.h>

typedef AST_Equation MMO_Equation;   // For now the same
typedef AST_Statement MMO_Statement;  // For now the same
typedef list<MMO_Equation> *MMO_EquationList;
typedef list<MMO_Statement> *MMO_StatementList;

/* AST_Class Wrapper */
class MMO_Class {
public:
  MMO_Class(AST_Class);
  void addEquation(MMO_Equation);
  MMO_EquationList getEquations();
private:
  MMO_EquationList _eqs;
  AST_Class _class;
};
