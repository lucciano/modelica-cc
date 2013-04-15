
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
	AST_CompositionElementList cel = comp->compositionList();
	AST_CompositionElementListIterator it;
	foreach(it,cel) {
		AST_EquationListIterator eqit;
		AST_CompositionElement e = current(it);
		AST_EquationList eqs = e->getEquations()->getEquations();
		foreach(eqit,eqs) 
			addEquation(current(eqit)); 
	}
	
	AST_ElementList el = comp->elementList();
	AST_ElementListIterator elit;
	foreach(elit,el) {
		switch (current(elit)->elementType()) {
			case COMPONENT:
				addComponent(current(elit)->getAsComponent());
				break;
			default:
				break;
		}
	}
	
}

void MMO_Class::addEquation(MMO_Equation e) {
	_eqs->push_back(e);
}

MMO_EquationList MMO_Class::getEquations() {
  return _eqs;
}

void MMO_Class::addComponent(MMO_Component c) {
	_comps->push_back(c);
}

MMO_ComponentList MMO_Class::getComponents() {
  return _comps;
}
