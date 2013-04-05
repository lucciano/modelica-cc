
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

#ifndef FLATTEN
#define FLATTEN

#include <ast/ast_builder.h>
#include <flatten/symbol_table.h>
#include <ast/equation.h>
class Flatten {
public:   
  Flatten();
  int flatten(AST_StoredDefinition sd);
  
private:
  void preFlat(AST_Class c);
  void doFlat(AST_Class c);
  void replaceComponentRefs(string name, AST_Equation, AST_Class c);
  void replaceComponentRefs(string name, AST_Expression, AST_Class c);
  AST_Component replaceComponent(string name, AST_Component);
  void expandConnections(AST_EquationConnect eq, AST_Class c) ;
  AST_StringList tsort(AST_Class);
  bool isLocalTo(string,AST_Class);
  AST_Equation duplicateEquation(AST_Equation e) { return e->duplicateEquation(); }
  SymbolTable components;
  SymbolTable types;
};

#endif
