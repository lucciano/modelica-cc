
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

MMO_Class_::MMO_Class_(AST_Class c, TypeSymbolTable ty):_class(c) {
	_eqs = 		new list<MMO_Equation>();
	_elems = 	new list<MMO_Element>();
	_stms = 	new list<MMO_Statement>();
	_Inieqs = 	new list<MMO_Equation>();
	_Inistms = 	new list<MMO_Statement>();
	
	varEnv = newVarSymbolTable();
	tyEnv = ty;
    varEnv->initialize(ty);
	
	AST_Composition comp = c->composition();

	// First elements
	AST_ElementList el = comp->elementList();
	AST_ElementListIterator elit;
	foreach(elit,el) {
		switch (current_element(elit)->elementType()) {
			case COMPONENT:
				addVariable(current_element(elit)->getAsComponent());
				break;
			default:
				addElement(current_element(elit));
				break;
		}
	}
	
	
	AST_CompositionElementList cel = comp->compositionList();
	AST_CompositionElementListIterator it;
	
	foreach(it,cel) {
		AST_EquationListIterator eqit;
		AST_ElementListIterator  elit;
		AST_StatementListIterator stit;
		
		// Equations 
		AST_CompositionElement e = current_element(it);
		AST_CompositionEqsAlgs eqA = e->getEquationsAlgs();
        if (eqA != NULL) {
			if  (eqA->isInitial()) {
				foreach(eqit,eqA->getEquations())  addIniEquation(current_element(eqit));
				foreach(stit,eqA->getAlgorithms()) addIniStatement(current_element(stit));
			} else {
				foreach(eqit,eqA->getEquations())  addEquation(current_element(eqit));
				foreach(stit,eqA->getAlgorithms()) addStatement(current_element(stit));
			}
		}
		
		// Elements 
		foreach(elit,e->getElementList()) {
			//AST_ListAppend(_comps , current_element(elit)->getAsComponent() );
			addVariable(current_element(elit)->getAsComponent());
		}
	}	
	_ct = new TypeCheck_( tyEnv , varEnv );
}

void MMO_Class_::addEquation(MMO_Equation e) {
	AST_ListAppend(_eqs,e);
}

void MMO_Class_::removeEquation(MMO_Equation e) {
	AST_ListRemove(_eqs,e);
}

MMO_EquationList MMO_Class_::getEquations() const {
  return _eqs;
}

void MMO_Class_::addIniEquation(MMO_Equation e) {
	AST_ListAppend(_Inieqs,e);
}


MMO_EquationList MMO_Class_::getIniEquations() const {
  return _Inieqs;
}


void MMO_Class_::addStatement(MMO_Statement e) {
	AST_ListAppend(_stms,e);
}

MMO_StatementList MMO_Class_::getStatements() const {
  return _stms;
}

void MMO_Class_::addIniStatement(MMO_Statement e) {
	AST_ListAppend(_Inistms,e);
}

MMO_StatementList MMO_Class_::getIniStatements() const {
  return _Inistms;
}


void MMO_Class_::addElement(MMO_Element c) {
	AST_ListAppend(_elems,c);
}

MMO_ElementList MMO_Class_::getElements() const {
  return _elems;
}


Type make_array_type(AST_ExpressionList dims , Type r)
{
	Type _t = r;
	AST_ExpressionListIterator exit;
	foreach(exit, dims){
		_t = new Type_Array_(_t , current_element(exit) );
	}
	return _t;
}


void MMO_Class_::addVariable(MMO_Component c)
{
	Type t = tyEnv->lookup(c->type());
	if (t == NULL) cerr << "No existe el tipo(" << c->type() << ")!!" << endl;
	
	AST_DeclarationListIterator it;
	foreach(it , c->nameList()) {
		
		AST_ExpressionList dims = newAST_ExpressionList();
		AST_ListConcat(dims, current_element(it)->indexes() );
		AST_ListConcat(dims, c->indexes() );
		
		if (dims->size() > 0 ) 
			t = make_array_type(  dims, t  );
		
		VarInfo  v = newVarInfo(t , c->typePrefix() , current_element(it)->modification(), current_element(it)->comment() );
		varEnv->insert(current_element(it)->name(), v);
	}
}

void MMO_Class_::addVariable(AST_String name , AST_String tys, AST_TypePrefix tp, AST_ExpressionList dims)
{
	Type t = tyEnv->lookup(toStr(tys));
	if (t == NULL) throw "addVariable: Nombre de tipo no encontrado" ;
	if (dims->size() > 0 ) 
			t = make_array_type(  dims, t  );
	VarInfo  v = newVarInfo(t , tp , NULL, NULL );
	varEnv->insert(toStr(name), v);
}

void MMO_Class_::addVariable(AST_String name , AST_String tys,AST_TypePrefix tp)
{
	Type t = tyEnv->lookup(toStr(tys));
	if (t == NULL) throw "addVariable: Nombre de tipo no encontrado" ;
	VarInfo  v = newVarInfo(t , tp , NULL, NULL );
	varEnv->insert(toStr(name), v);
}

void MMO_Class_::addVariable(AST_String name , AST_String tys, AST_TypePrefix tp,AST_Expression e)
{
	Type t = tyEnv->lookup(toStr(tys));
	if (t == NULL) throw "addVariable: Nombre de tipo no encontrado" ;
	
	AST_ArgumentList m = newAST_ArgumentList(); 
	AST_ListAppend(m,newAST_Argument_Modification(_S("start"),newAST_Modification_Equal(e)));
	VarInfo  v = newVarInfo(t , tp , newAST_Modification_Class(m , newAST_Expression_Null()) , NULL );
	varEnv->insert(toStr(name), v);
}

VarSymbolTable  MMO_Class_::getVarSymbolTable() const
{
	return varEnv;
}

TypeSymbolTable  MMO_Class_::getTypeSymbolTable()
{
	return tyEnv;
}

Type MMO_Class_::getExpresionType(AST_Expression e) 
{
	return _ct->check_expression(e);
}


VarInfo MMO_Class_::getVarInfo(AST_String name)
{
  if (varEnv->lookup(toStr(name))==NULL) return NULL; //throw ("getVarInfo: Variable " + toStr(name) + " no encontrada") ;
	return varEnv->lookup(toStr(name));
}

Type MMO_Class_::getVariableType(AST_String name)
{
	VarInfo v = getVarInfo(name);
	return v ? getVarInfo(name)->type() : NULL;
}


ostream & operator<<(ostream &ret  , const MMO_Class_ &c ) {
  MMO_EquationList  eqs = c.getEquations();
  MMO_StatementList stm = c.getStatements();
  MMO_EquationList  Inieqs = c.getIniEquations();
  MMO_StatementList Inistm = c.getIniStatements();
  MMO_ElementList 	ElemList = c.getElements();	
  ret << "model " << c.name() << endl;
  
  VarSymbolTable symbolTable = c.getVarSymbolTable();
  int i; int symbolTableSize = symbolTable->count();
  BEGIN_BLOCK;
  for (i = 0; i<symbolTableSize; i++) {
	  VarInfo var = symbolTable->varInfo(i);
    if (var->builtIn()) continue;
    MAKE_SPACE;
	  ret << *var  << " "  << symbolTable->varName(i);
  	if (var->modification()) ret <<  var-> modification() ;
  	if (var->comment()) ret <<  var-> comment() ;
	  ret  << ";" << endl;  
  }
  END_BLOCK;
  AST_ListPrint(ElemList,ret,"","","","",true);	
  
  AST_ListPrint(Inieqs,ret,"initial equation\n","","","",true);	
  AST_ListPrint(eqs,ret,"equation\n","","","",true);
  AST_ListPrint(Inistm,ret,"initial algorithm\n","","","",true);
  AST_ListPrint(stm,ret,"algorithm\n","","","",true);
  ret  << "end " << c.name() << ";" << endl;
  return ret;
}

ostream & operator<<(ostream &ret  , const MMO_Class &c ) 
{
	ret << *c;
	return ret;
}

MMO_Class newMMO_Class(AST_Class c, TypeSymbolTable t)
{
	return new MMO_Class_(c,t);
}

