
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
 
bool AST_Component_::isFlat() {
  cerr << "Checking if " << type() << " is flat= " << (type()=="Real") <<endl;
  if (type()=="Real") return true;
  return false;
};

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
