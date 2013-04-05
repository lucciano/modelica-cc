#include <ast/class.h>

AST_String AST_Class_::name() const { 
  return _name;
}
 
AST_Composition AST_Class_::composition() const { 
  return _composition; 
}

AST_ComponentList AST_Class_::getComponents() { 
  return _components; 
}

void AST_Class_::addComponent(AST_Component c) { 
  _components->push_back(c);
}

AST_Class_::AST_Class_(AST_String n,AST_Composition comp): _name(n), _composition(comp), _basic(false), _final(false) { 
}

bool AST_Class_::isBasic() { 
  return _basic; 
}

void AST_Class_::setComposition(AST_Composition c) { 
  _composition=c; 
}

void AST_Class_::setBasic() { 
  _basic=true; 
}

ostream & operator<<(ostream &os , const AST_Class &c ) {
  os << *c;
  return os;
}

ostream & operator<<(ostream &os , const AST_Class_ &cl )  {
    os << "class " << cl.name() << endl; 
    os << cl.composition();
    os<<"end " << cl.name() << ";" << endl;
}


AST_EquationList AST_Class_::getEquations(){ 
  return _equations; 
}

void AST_Class_::addEquation(AST_Equation e) { 
  _equations->push_back(e); 
}

void AST_Class_::removeEquation(AST_Equation e) { 
  _equations->remove(e); 
}

void AST_Class_::addClass(AST_Class c) { 
  _sub_classes->push_back(c); 
  c->setFather(this);
}          

AST_ClassList AST_Class_::getClasses() { 
  return _sub_classes;
}

void AST_Class_::setFather(AST_Class c) { 
  _father = c; 
}

AST_Class AST_Class_::father() const { 
  return _father; 
}

void AST_Class_::addExtends(AST_String e) {
  _extends_list->push_back(e);
}
AST_StringList AST_Class_::getExtends() {
  return _extends_list;
}
