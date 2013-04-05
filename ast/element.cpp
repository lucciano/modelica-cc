
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

#include <ast/element.h>
#include <ast/class.h>
#include <string>

using namespace std;

AST_Element_ClassWrapper_::AST_Element_ClassWrapper_(AST_Class_ *c): _c(c){
};

string AST_Element_ClassWrapper_::print() const { 
  stringstream ret(stringstream::out);
  if (_c==NULL) 
    ret << "CLASS[NULL]";
  else
    ret << (*_c);
return ret.str();
}
 
AST_Component_ *AST_Element_::getAsComponent() {
 return dynamic_cast<AST_Component_*>(this); 
}

AST_ExtendsClause AST_Element_::getAsExtendsClause() {
 return dynamic_cast<AST_ExtendsClause_*>(this); 
}

AST_Element_ClassWrapper AST_Element_::getAsClass() { 
  return dynamic_cast<AST_Element_ClassWrapper_*>(this) ;
};

ostream & operator<<(ostream &os , const AST_Component &c ){
    os << *c;
    return os;
}

ostream & operator<<(ostream &os , const AST_Element &e ) {
  os << *e;
  return os;
}

ostream & operator<<(ostream &os , const AST_Element_ &e ){
    os << e.print();  
    return os;
};

string AST_ExtendsClause_::print() const { 
    stringstream ret(stringstream::out);
    ret << "EXTENDS " << _name  ;
    return ret.str();
}

string AST_ImportClause_::print() const { 
    stringstream ret(stringstream::out);
    ret << "IMPORT[" << _name << "]";
    return ret.str();
}
AST_ImportClause_::AST_ImportClause_(string name):_name(name) {
}

AST_ExtendsClause_ ::AST_ExtendsClause_ (string name):_name(name) {
}

AST_Declaration_::AST_Declaration_(string name):_name(name) {}
;
string AST_Declaration_::print() const { 
    stringstream ret(stringstream::out);
    ret << _name;
    return ret.str();
}

AST_Component_::AST_Component_ (AST_DeclarationList decl_list, string type, AST_TypePrefix tp):_decl_list(decl_list), 
                                                                                          _type(type), 
                                                                                          _inherited(false), 
                                                                                          _origin(NULL),
                                                                                          _tp(tp) {
}
 
string AST_Component_::print() const {
  stringstream ret(stringstream::out);
    AST_DeclarationListIterator it,itt;
    ret << "  "; 
    if (isParameter()) 
      ret << "parameter "; 
    ret << _type << " "; 
    for (it=_decl_list->begin();it!=_decl_list->end();it++) {
      itt=it;
      itt++;
      ret << current(it) << (itt==_decl_list->end() ? "" : ",");
    }
    ret << ";"; 
    return ret.str();
}
