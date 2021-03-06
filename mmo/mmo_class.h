
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
#include <ast/modification.h>
#include <util/symbol_table.h>
#include <util/type_check.h>
#include <util/ast_util.h>

DEFINE_TYPE(MMO_Class);

typedef AST_Equation          	MMO_Equation;   // For now the same
typedef AST_Statement 		   	MMO_Statement;  // For now the same
typedef AST_Element_Component 	MMO_Component;  // For now the same
typedef AST_Element           	MMO_Element;

typedef list<MMO_Equation>  	*MMO_EquationList;
typedef list<MMO_Statement> 	*MMO_StatementList;
typedef list<MMO_Component>	*MMO_ComponentList;
typedef list<MMO_Element>		*MMO_ElementList;


typedef list<MMO_Equation>::iterator MMO_EquationListIterator;

#ifndef MMOCLASS_H
#define MMOCLASS_H

/* AST_Class Wrapper */
class MMO_Class_ {
public:
	MMO_Class_(AST_Class, TypeSymbolTable ty);
  
	void addEquation(MMO_Equation);
	MMO_EquationList getEquations() const; 
	void removeEquation(MMO_Equation);
  
	void addElement(MMO_Element);
	MMO_ElementList getElements() const;
  
	void addStatement(MMO_Statement);
	MMO_StatementList getStatements() const;
  
	void addIniEquation(MMO_Equation);
	MMO_EquationList getIniEquations() const; 
  
	void addIniStatement(MMO_Statement);
	MMO_StatementList getIniStatements() const;
  
  
	VarSymbolTable getVarSymbolTable() const;
	TypeSymbolTable getTypeSymbolTable();
  
	Type getExpresionType(AST_Expression e) ;
	Type getVariableType(AST_String name);
	VarInfo getVarInfo(AST_String name);
  
/* Agrega una variable */ 
	void addVariable(AST_String name,AST_String tys,AST_TypePrefix tp); 
  
/* Agrega una variable con modifications  (start = e) */ 
	void addVariable(AST_String name,AST_String tys,AST_TypePrefix tp, AST_Expression); 
  
/* Agrega una array con sus dimensiones y tipo */ 
	void addVariable(AST_String name,AST_String tys,AST_TypePrefix tp, AST_ExpressionList); 
  
  void setfsolve(AST_ClassList c);
  AST_ClassList fsolve() const;
	AST_String name() const { return _class->name(); }
  
	friend ostream & operator<<(ostream &os , const MMO_Class_ &c );
  
  void cleanComments();
  void sortParameters();
  AST_StringList parameters() const { return _parameters; }

private:
	/* Agrega una variables desde una Componet (solo para inicializar la clase) */
	void addVariable(MMO_Component); 
	MMO_EquationList _eqs;
	MMO_ElementList _elems;
	MMO_StatementList _stms;
  
	MMO_EquationList  _Inieqs;
	MMO_StatementList _Inistms;
  
	AST_Class _class;
  AST_ClassList _fsolve;
	VarSymbolTable  varEnv;
	TypeSymbolTable tyEnv;
	TypeCheck_ * _ct;	
  AST_StringList _parameters;
};

ostream & operator<<(ostream &os , const MMO_Class &c );
MMO_Class newMMO_Class(AST_Class, TypeSymbolTable);

MMO_EquationList newMMO_EquationList();

#endif
