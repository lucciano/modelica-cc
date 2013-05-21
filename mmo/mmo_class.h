
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
#include <util/symbol_table.h>
#include <util/type_check.h>


typedef AST_Equation  MMO_Equation;   // For now the same
typedef AST_Statement MMO_Statement;  // For now the same
typedef AST_Component MMO_Component;  // For now the same

typedef list<MMO_Equation>  *MMO_EquationList;
typedef list<MMO_Statement> *MMO_StatementList;
typedef list<MMO_Component>   *MMO_ComponentList;

typedef list<MMO_Equation>::iterator MMO_EquationListIterator;

#ifndef MMOCLASS_H
#define MMOCLASS_H
/* AST_Class Wrapper */
class MMO_Class {
public:
  MMO_Class(AST_Class, TypeSymbolTable ty);
  
  void addEquation(MMO_Equation);
  MMO_EquationList getEquations() const; 
  void removeEquation(MMO_Equation);
  
  void addComponent(MMO_Component);
  MMO_ComponentList getComponents();
  
  void addStatement(MMO_Statement);
  MMO_StatementList getStatements();
  
  VarSymbolTable getVarSymbolTable();
  TypeSymbolTable getTypeSymbolTable();
  Type getExpresionType(AST_Expression e) ;
  Type getVariableType(AST_String name);
  void addVariable(AST_String name,AST_String tys); 
  void addVariable(AST_String name,AST_String tys,AST_ExpressionList); 
  AST_String name() const { return _class->name(); }
  
  friend ostream & operator<<(ostream &os , const MMO_Class &c );

private:
  void addVariable(MMO_Component); 
  MMO_EquationList _eqs;
  MMO_ComponentList _comps;
  MMO_StatementList _stms;
  AST_Class _class;
  VarSymbolTable  varEnv;
  TypeSymbolTable tyEnv;
  TypeCheck_ * _ct;	
};

#endif
