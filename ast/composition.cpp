#include <ast/composition.h>
#include <ast/element.h>


list<AST_Equation_*> *AST_Composition_::getEquations() {
  list<AST_Equation_*> *ret= new list<AST_Equation_*>();
  list<AST_CompositionElement_*>::iterator it;
  for (it=_composition_list->begin();it!=_composition_list->end();it++) {
    if ((*it)->_eqs!=NULL)  
      ret->insert(ret->end(),(*it)->_eqs->begin(),(*it)->_eqs->end());
  }
  return ret;
}

AST_EquationList AST_CompositionElement_::getEquations() { return _eqs; }

AST_CompositionElementList AST_Composition_::compositionList() const { 
  return _composition_list; 
}

AST_ElementList AST_Composition_::elementList() const { 
  return _element_list;
}

ostream & operator<<(ostream &os , const AST_Composition &c ) {
  os << *c;
  return os;
}

ostream & operator<<(ostream &os , const AST_Composition_ &cm )  {
    AST_CompositionElementListIterator it;
    AST_ElementListIterator el_it;
    foreach(el_it,cm.elementList()) 
      os << current(el_it) << endl;
    foreach (it,cm.compositionList())
      os << current(it);
    return os;

}
