
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

#include <sstream>
#include <list>

#include <ast/ast_node.h>
#include <ast/expression.h>
/*const char *BinOpTypeName[] =  {"BINOPLOWER", "BINOPLOWEREQ", "BINOPGREATER", "BINOPGREATEREQ", "BINOPCOMPNE", "BINOPCOMPEQ", "BINOPDIV", "BINOPELDIV", "BINOPMULT",
                          "BINOPELMULT", "BINOPADD", "BINOPELADD", "BINOPSUB", "BINOPELSUB","BINOPEXP", "BINOPELEXP", "BINOPAND", "BINOPOR" };
*/

const char *BinOpTypeName[] =  {"<", "<=", ">", ">=", "<>", "==", "/", "./", "*",
                          ".*", "+", ".+", "-", ".-","^", ".^", " and ", " or " };

using namespace std;
ostream & operator<<(ostream &os , const AST_Expression_ &e ){
    os << e.print();  
    return os;
}

ostream & operator<<(ostream &os , const AST_Expression &e ){
    os << *e;
    return os;
}
AST_Expression_BinOp_::AST_Expression_BinOp_(AST_Expression e1,AST_Expression e2, BinOpType t): _e1(e1), _e2(e2), _t(t) {
}


string AST_Expression_BinOp_::print() const { 
    stringstream ret(stringstream::out);
    ret  << left() << BinOpTypeName[binopType()] << right() ;
    return ret.str();
}

AST_Expression_Integer_::AST_Expression_Integer_(int i):_i(i) {
};


string AST_Expression_Integer_::print() const { 
  stringstream ret(stringstream::out);
  //ret << "INTEGER[" << _i << "]";
  ret << _i ;
  return ret.str();
}

AST_Expression_Real_::AST_Expression_Real_(double d):_d(d) {
}

string AST_Expression_Real_::print() const { 
  stringstream ret(stringstream::out);
  ret <<  _d;
  return ret.str();
}

AST_Expression_String_::AST_Expression_String_(string s): _s(s){
}

string AST_Expression_String_::print() const { 
  stringstream ret(stringstream::out);
  ret << "STRING[" << _s << "]";
  return ret.str();
}

AST_Expression_Boolean_::AST_Expression_Boolean_(bool b):_b(b){
}

string AST_Expression_Boolean_::print() const { 
  stringstream ret(stringstream::out);
  ret << "BOOLEAN[" << (_b ? "true" : "false") << "]";
  return ret.str();
}

AST_Expression_Derivative_::AST_Expression_Derivative_(AST_ExpressionList el):_el(el){
}

string AST_Expression_Derivative_::print() const { 
  stringstream ret(stringstream::out);
  ret << "der(" << *_el->front() << ")";
  return ret.str();
};

AST_Expression_UMinus_::AST_Expression_UMinus_(AST_Expression e): _e(e){
};

string AST_Expression_UMinus_::print() const { 
  stringstream ret(stringstream::out);
  ret << "-(" <<  _e << ")";
  return ret.str();
};

AST_Expression_BooleanNot_::AST_Expression_BooleanNot_(AST_Expression e):_e(e){};
string AST_Expression_BooleanNot_::print() const { 
  stringstream ret(stringstream::out);
  ret << "not " <<  exp();
  return ret.str();
}

AST_Expression_Call_::AST_Expression_Call_(AST_String name, AST_ExpressionList args):_name(name), _args(args) { };

string AST_Expression_Call_::print() const {
  stringstream ret;
  AST_ExpressionListIterator it;
  ret << name() << "(" ;
  int size = arguments()->size(),i=0;
  foreach(it,arguments()) {   
    i++;
    ret << current(it);
    ret << (i<size ? "," : "");
  }
  ret << ")";
  return ret.str();
}

AST_Expression_ComponentReference_::AST_Expression_ComponentReference_(string name):_name(name){};
AST_Expression_ComponentReference_::AST_Expression_ComponentReference_():_name(""){};
string AST_Expression_ComponentReference_::print() const {
  stringstream ret;
  //ret << "COMPONENTREF[" << _name << "]";
  ret <<  _name ;
  return ret.str();
}
#include <stdlib.h>
void AST_Expression_ComponentReference_::append(AST_Expression_ComponentReference_ *cr) { 
  _name.append(cr->_name);
}

void AST_Expression_ComponentReference_::append(string s) {
  _name.append(s);
}


AST_Expression_ComponentReference AST_Expression_::getAsComponentRef() {
  return dynamic_cast<AST_Expression_ComponentReference>(this);
}


AST_Expression_Range AST_Expression_::getAsRange() {
  return dynamic_cast<AST_Expression_Range>(this);
}

AST_Expression_If AST_Expression_::getAsIf() {
  return dynamic_cast<AST_Expression_If>(this);
}

AST_Expression_UMinus AST_Expression_::getAsUMinus() {
  return dynamic_cast<AST_Expression_UMinus>(this);
}

AST_Expression_Output AST_Expression_::getAsOutput() {
  return dynamic_cast<AST_Expression_Output>(this);
}

AST_Expression_BooleanNot AST_Expression_::getAsBooleanNot() {
  return dynamic_cast<AST_Expression_BooleanNot>(this);
}

void AST_Expression_ComponentReference_::setName(string name) { _name=name; }

ExpressionType AST_Expression_BooleanNot_::expressionType() { 
  return EXPBOOLEANNOT; 
}

ExpressionType AST_Expression_::expressionType() { 
  return EXPNONE; 
}

ExpressionType AST_Expression_UMinus_::expressionType() { 
  return EXPUMINUS;
}

ExpressionType AST_Expression_Boolean_::expressionType() { 
  return EXPBOOLEAN;
}

ExpressionType AST_Expression_Integer_::expressionType() { 
  return EXPINTEGER;
}

ExpressionType AST_Expression_Real_::expressionType() { 
  return EXPREAL;
}

ExpressionType AST_Expression_String_::expressionType() { 
  return EXPSTRING;
}

AST_Integer AST_Expression_Integer_::val() {
  return _i;
}

AST_Expression_Derivative AST_Expression_::getAsDerivative() {
  return dynamic_cast<AST_Expression_Derivative>(this);
};

AST_Expression_If_ElseIf AST_Expression_::getAsElseIf() {
  return dynamic_cast<AST_Expression_If_ElseIf>(this);
};

/* If expression */
AST_Expression_If_::AST_Expression_If_(AST_Expression cond, AST_Expression then, AST_Expression else_exp, AST_ExpressionList elseif_list): _cond(cond), 
                                                                                                           _then(then), 
                                                                                                           _else_exp(else_exp),
                                                                                                           _elseif_list(elseif_list) {
}
AST_Expression_If_ElseIf_::AST_Expression_If_ElseIf_  (AST_Expression c, AST_Expression t) : _cond(c) , _then(t) {};


string AST_Expression_If_ElseIf_::print() const {
	stringstream ret;
	ret << " elseif " << _cond << " then " << _then ;
	return ret.str();
}

string AST_Expression_If_::print () const {
  stringstream ret;
  AST_ExpressionListIterator it;
  ret << "if "<< condition() << " then " << then() ;
  if (! elseif_list()->empty())
  foreach(it , elseif_list() )
	  ret << current(it)->getAsElseIf() ;
  ret << " else " << else_exp() ;
  return ret.str();
}

ExpressionType AST_Expression_If_::expressionType() {
  return EXPIF;
}

ExpressionType AST_Expression_Call_::expressionType() {
  return EXPCALL;
}

string AST_Expression_End_::print() const { 
  return "end";
}


AST_Expression_BinOp AST_Expression_::getAsBinOp() {
  dynamic_cast<AST_Expression_BinOp>(this);
}

AST_Expression_Output_::AST_Expression_Output_(AST_ExpressionList l): _list(l) {
}

AST_Expression_Range_::AST_Expression_Range_(AST_ExpressionList l): _list(l) {
}

string AST_Expression_Range_::print () const {
  AST_ExpressionListIterator it;
  stringstream ret;
  int size=_list->size(),i=0;
  foreach (it,_list) {
    i++;
    ret << current(it);
    ret << (i==size ? "" : ":");
  }
  return ret.str();

}

string AST_Expression_Output_::print () const {
  AST_ExpressionListIterator it;
  stringstream ret;
  ret << "(";
  int size=_list->size(),i=0;
  foreach (it,_list) {
    i++;
    ret << current(it);
    ret << (i==size ? "" : ",");
  }
  ret << ")";
  return ret.str();
  
}

AST_ExpressionList AST_Expression_Output_::expressionList()
{
	return _list;
}

AST_ExpressionList AST_Expression_Range_::expressionList()
{
	return _list;
}


ostream & operator<<(ostream &os , const AST_Expression_ComponentReference &cr) {
  os << *cr;
  return os;
}

