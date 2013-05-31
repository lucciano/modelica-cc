
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

#include <ast/equation.h>
#include <ast/ast_builder.h>


AST_Equation_Equality AST_Equation_::getAsEquality() {
  return dynamic_cast<AST_Equation_Equality_*>(this);
}

AST_Equation_Connect AST_Equation_::getAsConnect() {
  return dynamic_cast<AST_Equation_Connect_*>(this);
}

AST_Equation_When AST_Equation_::getAsWhen() {
	return dynamic_cast<AST_Equation_When_*>(this);
}

AST_Equation_If AST_Equation_::getAsIf() {
	return dynamic_cast<AST_Equation_If_*>(this);
}

AST_Equation_For AST_Equation_::getAsFor() {
	return dynamic_cast<AST_Equation_For_*>(this);
}

AST_Equation_Call AST_Equation_::getAsCall() {
	return dynamic_cast<AST_Equation_Call_*>(this);
}

AST_Expression AST_Equation_Equality_::left() const {
  return _left;
}
AST_Expression AST_Equation_Equality_::right() const {
  return _right;
}


string AST_Equation_When_::print() const {
  stringstream ret(stringstream::out);
  AST_EquationList eql = equationList();
  AST_EquationListIterator it;
  AST_Equation_ElseListIterator else_it;
  MAKE_SPACE;
  ret << "when " << condition() << " then" << endl;
  BEGIN_BLOCK;
  foreach(it,eql) 
    ret << current(it);
  END_BLOCK;
  foreach(else_it,equationElseWhen()) {
    MAKE_SPACE;
    ret << "elsewhen " << current(else_it)->condition() << " then" << endl ;
    BEGIN_BLOCK;
    foreach(it, current(else_it)->equations())
      ret << current(it);
    END_BLOCK;
  }
 
  MAKE_SPACE;
  ret << "end when;" << endl;
  return ret.str();
}


string AST_Equation_If_::print() const {
  stringstream ret(stringstream::out);
  AST_EquationListIterator it;
  AST_Equation_ElseListIterator else_it;
  MAKE_SPACE;
  ret << "if (" << condition() << ") " << endl;
  BEGIN_BLOCK;
  foreach(it,equationList())
    ret << current(it);
  END_BLOCK;
  foreach(else_it,equationElseIf()) {
    MAKE_SPACE;
    ret << "elseif " << current(else_it)->condition() << " then" << endl ;
    BEGIN_BLOCK;
    foreach(it, current(else_it)->equations())
      ret << current(it);
    END_BLOCK;

  }
  if (equationElseList()->size()) { 
    MAKE_SPACE;
    ret << "else" << endl;
    BEGIN_BLOCK;
    foreach(it,equationElseList()) 
      ret << current(it);
    END_BLOCK;
  }
  MAKE_SPACE;
  ret << "end if" << endl;
  return ret.str();
}

ostream & operator<<(ostream &os , const AST_Equation &e ) {
  os << *e;
  return os;
}

ostream & operator<<(ostream &os , const AST_ForIndex &f ) {
  os << *f;
  return os;
}

ostream & operator<<(ostream &os , const AST_ForIndex_ &f ) {
  os << f.variable();
  if (f.in_exp()!=NULL) { 
    os << " in " << f.in_exp();
  }
  return os;
}






AST_Equation_Call_::AST_Equation_Call_(AST_Expression e):_call(e) {

}


AST_Expression AST_Equation_Call_::call () const { 
  return _call;
}

string AST_Equation_Call_::print () const {
  stringstream ret(stringstream::out);
  MAKE_SPACE;
  ret << call() << ";" << endl;
  return ret.str();
}


string AST_Equation_For_::print() const {
  stringstream ret(stringstream::out);
  AST_EquationList eql = equationList();
  AST_EquationListIterator it;
  AST_ForIndexListIterator ind_it;
  MAKE_SPACE;
  AST_ListPrint(forIndexList(),ret,"for ",",",""," loop\n",false);
  BEGIN_BLOCK;
  foreach(it,eql)
    ret << current(it);
  END_BLOCK;
  MAKE_SPACE;
  ret << "end for;"<< endl;
  return ret.str();

  return ret.str();
}

ostream & operator<<(ostream &os , const AST_Equation_ &e ){
    os << e.print();  
    return os;
}

string AST_Equation_Equality_::print() const {
  stringstream ret(stringstream::out);
  MAKE_SPACE;
  ret << left() << " = " << right() << ";" << endl;
  return ret.str();
}

void AST_Equation_Equality_::setLeft(AST_Expression e) {
  _left=e;
}

void AST_Equation_Equality_::setRight(AST_Expression e) {
  _right=e;
}

AST_Equation_Equality_::AST_Equation_Equality_ (AST_Expression left, AST_Expression right):_left(left), _right(right) {
}

AST_Equation_Connect_::AST_Equation_Connect_(AST_Expression_ComponentReference cr1, AST_Expression_ComponentReference cr2):_cr1(cr1), _cr2(cr2) {

}
string AST_Equation_Connect_::print() const {
  stringstream ret(stringstream::out);
  MAKE_SPACE;
  ret << "connect(" << _cr1 << "," << _cr2 <<");" << endl;
  return ret.str();
}
