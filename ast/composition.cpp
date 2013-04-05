#include <assert.h>
#include <stdio.h>
#include <ast/composition.h>
#include <ast/element.h>


list<AST_Equation_*> *AST_Composition_::getEquations() {
  list<AST_Equation_*> *ret= new list<AST_Equation_*>();
  list<AST_CompositionElement_*>::iterator it;
  /*
  for (it=_composition_list->begin();it!=_composition_list->end();it++) {
    if ((*it)->_eqs_algs!=NULL)  
      ret->insert(ret->end(),(*it)->_eqs_algs->begin(),(*it)->_eqs_algs->end());
  }
  */
  return ret;
}

AST_CompositionEqsAlgs AST_CompositionElement_::getEquations() { return _eqs_algs; }

AST_CompositionElementList AST_Composition_::compositionList() const { 
  return _composition_list; 
}

AST_ElementList AST_Composition_::elementList() const { 
  return _element_list;
}

ostream & operator<<(ostream &os , const AST_CompositionElement_ &ce ) 
{ 
  AST_EquationListIterator it;
  os << "equation" << endl;
  foreach (it,ce._eqs_algs->getEquations())  {
      os << current(it); 
  }
  return os;
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
      os << current(el_it) << endl;
    foreach (it,cm.compositionList()) {
      os << current(it);
    }
    return os;

}


AST_CompositionEqsAlgs_::AST_CompositionEqsAlgs_(AST_EquationList eq): _eq(eq) {
}

ostream & operator<<(ostream &os , const AST_CompositionEqsAlgs &ceqa ) 
{
  assert(ceqa!=NULL);
  os << *ceqa;
  return os;
}

