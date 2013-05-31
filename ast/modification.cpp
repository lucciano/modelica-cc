
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

#include <ast/modification.h>
#include <ast/expression.h>
#include <sstream>

AST_ModificationEqual_:: AST_ModificationEqual_(AST_Expression e): _e(e) {
}

AST_ModificationAssign_:: AST_ModificationAssign_(AST_Expression e): _e(e) {
}

AST_ModificationClass_:: AST_ModificationClass_(AST_ArgumentList al,AST_Expression e): _e(e), _al(al) {
}

ostream & operator<<(ostream &os , const AST_Modification_ &e ){
    os << e.print();  
    return os;
}

ostream & operator<<(ostream &os , const AST_Modification &e ){
    os << *e;
    return os;
}


GET_AS_IMP(Modification,Equal);
GET_AS_IMP(Modification,Class);
GET_AS_IMP(Modification,Assign);
GET_AS_IMP(Argument,Modification);


string AST_ModificationEqual_::print() const { 
  stringstream ret(stringstream::out);
  ret << " = " << _e ;
  return ret.str();
}

string AST_ModificationAssign_::print() const { 
  stringstream ret(stringstream::out);
  ret << " := " << _e;
  return ret.str();
}

string AST_ModificationClass_::print() const { 
  stringstream ret(stringstream::out);
  
  
  AST_ListPrint(arguments(),ret,"",",","(",")");
  /*AST_ArgumentListIterator it;
  if (arguments()->size()) {
    ret << "(";
    foreach(it,arguments()) 
      ret << current(it);
    ret << ")";
  } */
  if (exp()->expressionType()!=EXPNULL) {
    ret << " = " << exp();
  }
  
  return ret.str();
}


ostream & operator<<(ostream &os , const AST_Argument_ &a ){
    os << a.print();  
    return os;
}

ostream & operator<<(ostream &os , const AST_Argument &a ){
    os << *a;
    return os;
}



AST_ArgumentModification_::AST_ArgumentModification_(AST_String name, AST_Modification m): _name(name), _mod(m) {

}
string AST_ArgumentModification_::print() const { 
  stringstream ret(stringstream::out);
  if (modification()!=NULL)
    ret << name() << modification();
  else
    ret << name();
  return ret.str();
}


