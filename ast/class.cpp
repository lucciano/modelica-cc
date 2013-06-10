
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

#include <ast/class.h>

CLASSP_PRINTER_IMP(AST_Class);

AST_String AST_Class_::name() const {
  return _name;
}

AST_Composition AST_Class_::composition() const {
  return _composition;
}

AST_Element_ComponentList AST_Class_::getComponents() {
  return _components;
}

void AST_Class_::addComponent(AST_Element_Component c) {
  AST_ListAppend(_components,c);
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


ostream & operator<<(ostream &ret , const AST_Class_ &cl )  {

  MAKE_SPACE;
  switch (cl.prefix()) {
  case CP_CLASS:
    ret << "class " ;
    break;
  case CP_MODEL:
    ret << "model " ;
    break;
  case CP_FUNCTION:
    ret  << "function " ;
    break;
  }
  ret << cl.name() << endl;
  BEGIN_BLOCK;
  ret << cl.composition();
  END_BLOCK;
  MAKE_SPACE;
  ret <<"end " << cl.name() << ";" << endl;
  return ret;
}

void AST_Class_::addClass(AST_Class c) {
  AST_ListAppend(_sub_classes,c);
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
  AST_ListAppend(_extends_list,e);
}
AST_StringList AST_Class_::getExtends() {
  return _extends_list;
}

void AST_Class_::setFinal() {
  _final=true;
}

void AST_Class_::setEncapsulated() {
  _encapsulated=true;
}

void AST_Class_::setPrefixes(AST_ClassPrefix cp) {
  _prefix=cp;
}

AST_ClassPrefix AST_Class_::prefix() const {
  return _prefix;
}
