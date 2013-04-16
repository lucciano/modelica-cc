
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

MMO_Class::MMO_Class(AST_Class c):_class(c) {
	_eqs = new list<MMO_Equation>();
	_comps = new list<MMO_Component>();
	
	AST_Composition comp = c->composition();
	
	// First elements
	AST_ElementList el = comp->elementList();
	AST_ElementListIterator elit;
	foreach(elit,el) {
		switch (current(elit)->elementType()) {
			case COMPONENT:
				_comps->push_back(current(elit)->getAsComponent());
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
				_eqs->push_back(current(eqit)); 
			}
		}
		
		// Elements 
		foreach(elit,e->getElementList()) {
			_comps->push_back(current(elit)->getAsComponent()); 	
		}
	}	
}

void MMO_Class::addEquation(MMO_Equation e) {
	_eqs->push_front(e);
	// For now
	_class->composition()->compositionList()->front()->getEquationsAlgs()->getEquations()->push_back(e); 
}

MMO_EquationList MMO_Class::getEquations() {
  return _eqs;
}

void MMO_Class::addComponent(MMO_Component c) {
	_comps->push_back(c);
	_class->composition()->elementList()->push_back(c);
	
}

MMO_ComponentList MMO_Class::getComponents() {
  return _comps;
}
