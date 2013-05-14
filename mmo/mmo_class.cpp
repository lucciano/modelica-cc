
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

#include <mmo/mmo_class.h>

#include <iostream>
using namespace std;

MMO_Class::MMO_Class(AST_Class c, TypeSymbolTable ty):_class(c) {
	_eqs = new list<MMO_Equation>();
	_comps = new list<MMO_Component>();
	_stms = new list<MMO_Statement>();
	varEnv = new VarSymbolTable_;
	tyEnv = ty;
  varEnv->initialize(ty);
	
	AST_Composition comp = c->composition();

	// First elements
	AST_ElementList el = comp->elementList();
	AST_ElementListIterator elit;
	foreach(elit,el) {
		switch (current(elit)->elementType()) {
			case COMPONENT:
			
				AST_ListAppend(_comps , current(elit)->getAsComponent() ) ;
				addVariable(current(elit)->getAsComponent());
				break;
			default:
				break;
		}
	}
	
	
	AST_CompositionElementList cel = comp->compositionList();
	AST_CompositionElementListIterator it;
	
	foreach(it,cel) {
		AST_EquationListIterator eqit;
		AST_ElementListIterator  elit;
		
		// Equations 
		AST_CompositionElement e = current(it);
        if (e->getEquationsAlgs() != NULL) {
			AST_EquationList eqs = e->getEquationsAlgs()->getEquations();
			foreach(eqit,eqs) {
				AST_ListAppend(_eqs , current(eqit));
			}
		}
		
		// Elements 
		foreach(elit,e->getElementList()) {
			AST_ListAppend(_comps , current(elit)->getAsComponent() );
			addVariable(current(elit)->getAsComponent());
		}
	}	
	_ct = new TypeCheck_( tyEnv , varEnv );
}

void MMO_Class::addEquation(MMO_Equation e) {
	AST_ListAppend(_eqs,e);
}

void MMO_Class::removeEquation(MMO_Equation e) {
	AST_ListRemove(_eqs,e);
}

MMO_EquationList MMO_Class::getEquations() const {
  return _eqs;
}

void MMO_Class::addStatement(MMO_Statement e) {
	AST_ListAppend(_stms,e);
}

MMO_StatementList MMO_Class::getStatements() {
  return _stms;
}

void MMO_Class::addComponent(MMO_Component c) {
	AST_ListAppend(_comps,c);
}

MMO_ComponentList MMO_Class::getComponents() {
  return _comps;
}


Type make_array_type(AST_ExpressionList dims , Type r)
{
	Type _t = r;
	AST_ExpressionListIterator exit;
	foreach(exit, dims){
		_t = new Type_Array_(_t , current(exit) );
	}
	return _t;
}


void MMO_Class::addVariable(MMO_Component c)
{
	Type t = tyEnv->lookup(c->type());
	if (t == NULL) cerr << "No existe el tipo(" << c->type() << ")!!" << endl;
	
	AST_DeclarationListIterator it;
	foreach(it , c->nameList()) {
		
		AST_ExpressionList dims = newAST_ExpressionList();
		AST_ListConcat(dims, current(it)->indexes() );
		AST_ListConcat(dims, c->indexes() );
		
		if (dims->size() > 0 ) 
			t = make_array_type(  dims, t  );
		
		VarInfo * v = new VarInfo(t , c->typePrefix());
		varEnv->insert(current(it)->name(), v);
	}
}

void MMO_Class::addVariable(AST_String name , AST_String tys)
{
	Type t = tyEnv->lookup(*tys);
	if (t == NULL) cerr << "No existe el tipo(" << tys << ")!!" << endl;
	VarInfo * v = new VarInfo(t , newAST_TypePrefix() );
	varEnv->insert(*name, v);
}

VarSymbolTable  MMO_Class::getVarSymbolTable()
{
	return varEnv;
}

TypeSymbolTable  MMO_Class::getTypeSymbolTable()
{
	return tyEnv;
}

Type MMO_Class::getExpresionType(AST_Expression e) {
	return _ct->check_expression(e);
}

Type MMO_Class::getVariableType(AST_String name)
{
	Type t = varEnv->lookup(*name)->type();
}

ostream & operator<<(ostream &ret  , const MMO_Class &c ) {
  MMO_EquationList eqs = c.getEquations();
  AST_EquationListIterator it;
  ret << "class " << c.name() << endl;
  if (eqs->size())
    ret  << "equaiton" << endl;
  BEGIN_BLOCK;
  foreach(it,eqs) 
    ret << current(it);
  END_BLOCK;
  ret  << "end " << c.name() << ";" << endl;
  return ret;
}
