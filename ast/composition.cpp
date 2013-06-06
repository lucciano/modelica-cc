
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

#include <assert.h>
#include <stdio.h>
#include <ast/composition.h>
#include <ast/element.h>


AST_CompositionElement_::AST_CompositionElement_(AST_CompositionEqsAlgs eqs_algs): _eqs_algs(eqs_algs), _el(newAST_ElementList()) {
}

AST_CompositionElement_::AST_CompositionElement_(AST_ElementList el):  _eqs_algs(newAST_NullCompositionEquations()) , _el(el){ 
}


AST_Composition_ :: AST_Composition_ (AST_ElementList el,AST_CompositionElementList cl): _composition_list(cl), _element_list(el) {
}

AST_CompositionEqsAlgs AST_CompositionElement_::getEquationsAlgs() { 
  return _eqs_algs; 
}

AST_ElementList AST_CompositionElement_::getElementList() { 
  return _el; 
}

AST_CompositionElementList AST_Composition_::compositionList() const { 
  return _composition_list; 
}

AST_ElementList AST_Composition_::elementList() const { 
  return _element_list;
}

ostream & operator<<(ostream &ret , const AST_CompositionElement_ &ce ) 
{ 
  AST_EquationListIterator it;
  AST_StatementListIterator st_it;
  if (ce._eqs_algs!=NULL) {
    if (ce._eqs_algs->getEquations()->size()>0) {
      MAKE_SPACE;
      ret  << (ce._eqs_algs->isInitial() ? "initial " : "");
      ret << "equation" << endl;
    }
    BEGIN_BLOCK;
    foreach (it,ce._eqs_algs->getEquations())  
        ret << current_element(it);
    END_BLOCK;
    if (ce._eqs_algs->getAlgorithms()->size()>0) {
      MAKE_SPACE;
      ret  << (ce._eqs_algs->isInitial() ? "initial " : "");
      ret << "algorithm" << endl;
    }
    BEGIN_BLOCK;
    foreach (st_it,ce._eqs_algs->getAlgorithms())  
        ret << current_element(st_it);
    END_BLOCK;
  }
  AST_ElementListIterator et;
  if (ce._el != NULL) {
	if (ce._el->size() > 0)
		ret  << "public" << endl;
	foreach (et,ce._el )  
        ret << "  " << current_element(et) << endl;
  }
  
  return ret;
} 
 
ostream & operator<<(ostream &os , const AST_Composition &c ) {
  assert(c!=NULL);
  os << *c;
  return os;
}

ostream & operator<<(ostream &os , const AST_Composition_ &cm )  {
    AST_CompositionElementListIterator  it;
    AST_ElementListIterator             el_it;
    foreach(el_it,cm.elementList()) 
      os << current_element(el_it) << endl;
    foreach (it,cm.compositionList()) {
      os << current_element(it);
    }
    return os;
}


AST_CompositionEqsAlgs_::AST_CompositionEqsAlgs_(AST_EquationList eq):  _initial(false) ,_eq(eq), _st(newAST_StatementList()){
}

AST_CompositionEqsAlgs_::AST_CompositionEqsAlgs_(AST_EquationList eq, bool i):  _initial(i) ,_eq(eq), _st(newAST_StatementList()) {
}

AST_CompositionEqsAlgs_::AST_CompositionEqsAlgs_(AST_StatementList st, bool i):   _initial(i) ,_eq(newAST_EquationList()),_st(st) {
}

AST_CompositionEqsAlgs_::AST_CompositionEqsAlgs_(AST_StatementList st):  _initial(false) , _eq(newAST_EquationList()) , _st(st){
}
ostream & operator<<(ostream &os , const AST_CompositionEqsAlgs &ceqa ) 
{
  assert(ceqa!=NULL);
  os << *ceqa;
  return os;
}

