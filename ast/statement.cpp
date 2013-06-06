
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

#include <ast/ast_builder.h>
#include <ast/statement.h>
#include <ast/expression.h>
#include <iostream>
#include <sstream>


using namespace std;

GET_AS_IMP(Statement,_While);
GET_AS_IMP(Statement,_If);
GET_AS_IMP(Statement,_For);
GET_AS_IMP(Statement,_When);
GET_AS_IMP(Statement,_Assign);

ostream & operator<<(ostream &os , const AST_Statement_ &s ) {
  os << s.print();  
  return os;
}

string AST_Statement_Return_::print() const { 
  stringstream ret(stringstream::out);
  MAKE_SPACE;
  ret << "return" << endl; 
  return ret.str();
}

string AST_Statement_Break_::print() const { 
  stringstream ret(stringstream::out);
  MAKE_SPACE;
  ret << "break" << endl; 
  return ret.str();
}

string AST_Statement_Assign_::print() const { 
  stringstream ret(stringstream::out);
  AST_ExpressionListIterator it;
  MAKE_SPACE;
  if (exp()->expressionType()!=EXPCALLARG)
    ret << lhs() << ":=" << exp() << ";"<<endl;
  else if (exp()->expressionType()==EXPCALLARG) {	   	  
    ret << lhs() << "(";
    int size=exp()->getAsCallArgs()->arguments()->size(),i=0;
    foreach(it,exp()->getAsCallArgs()->arguments()) {
      ret << current_element(it) << (++i<size ? "," : "");
    }
    ret << ");" << endl;
  }
    
  return ret.str();
}
AST_Statement_Assign_::AST_Statement_Assign_(AST_Expression_ComponentReference lhs,AST_Expression exp): _lhs(lhs), _exp(exp) {
}


AST_Statement_For_::AST_Statement_For_(AST_ForIndexList index, AST_StatementList sts): _sts(sts),_ind(index) {
}

string AST_Statement_For_::print() const { 
  stringstream ret(stringstream::out);
  AST_StatementListIterator it;
  MAKE_SPACE;
  AST_ListPrint(forIndexList(),ret,"for ",",",""," loop\n",false);
  BEGIN_BLOCK;
  foreach(it,statements())
    ret << current_element(it);
  END_BLOCK;
  MAKE_SPACE;
  ret << "end for;" <<endl;
  return ret.str();
}
  
string AST_Statement_When_::print() const { 
  stringstream ret(stringstream::out);
  AST_StatementListIterator it;
  AST_Statement_ElseListIterator else_it;
  
  MAKE_SPACE;
  ret << "when " << condition() << " then"<<endl;
  
  BEGIN_BLOCK;
  foreach(it,statements())
    ret << current_element(it);
  END_BLOCK;
  foreach(else_it,else_when()) {
    MAKE_SPACE;
    ret << "elsewhen " << current_element(else_it)->condition() << " then" << endl ;
    BEGIN_BLOCK;
    foreach(it, current_element(else_it)->statements())
      ret << current_element(it);
    END_BLOCK;
  }
 
  MAKE_SPACE;
  ret << "end when;" <<endl;
  return ret.str();
}

string AST_Statement_While_::print() const { 
  stringstream ret(stringstream::out);
  AST_StatementListIterator it;
  MAKE_SPACE;
  ret << "while " << condition() << " loop"<<endl;
  BEGIN_BLOCK;
  foreach(it,statements())
    ret << current_element(it);
  END_BLOCK;
  MAKE_SPACE;
  ret << "end while;" <<endl;
  return ret.str();
}


AST_Statement_When_::AST_Statement_When_(AST_Expression cond, AST_StatementList sts, AST_Statement_ElseList else_st): _cond(cond), _sts(sts),_else_list(else_st) {
}

AST_Statement_While_::AST_Statement_While_(AST_Expression cond, AST_StatementList sts): _cond(cond), _sts(sts) {
}

ostream & operator<<(ostream &os , const AST_Statement &s ) {
  os << *s;
  return os;
}


AST_Statement_If_::AST_Statement_If_(AST_Expression cond, AST_StatementList true_st, AST_Statement_ElseList else_list, AST_StatementList false_st): 
  _cond(cond), _true_st(true_st), _false_st(false_st), _else_list(else_list){
}
string AST_Statement_If_::print() const {
  stringstream ret(stringstream::out);
  AST_StatementListIterator it;
  AST_Statement_ElseListIterator else_it;
  MAKE_SPACE;
  ret << "if " << condition() << " then"<<endl;
  BEGIN_BLOCK;
  foreach(it,statements())
    ret << current_element(it);
  END_BLOCK;
  foreach(else_it,else_if()) {
    MAKE_SPACE;
    ret << "elseif " << current_element(else_it)->condition() << " then" << endl ;
    BEGIN_BLOCK;
    foreach(it, current_element(else_it)->statements())
      ret << current_element(it);
    END_BLOCK;
  }
 
  if (else_statements()->size()) {
    MAKE_SPACE;
    ret << "else" << endl;
  }
  BEGIN_BLOCK;
  foreach(it,else_statements())
    ret << current_element(it);
  END_BLOCK;
  MAKE_SPACE;
  ret << "end if;" <<endl;
  return ret.str();

}

AST_Statement_OutputAssigment_:: AST_Statement_OutputAssigment_(AST_ExpressionList out_exps, AST_Expression_ComponentReference func, AST_ExpressionList args): 
  _out_exps(out_exps),_func(func),_args(args) {
}

string AST_Statement_OutputAssigment_::print () const {
  stringstream ret(stringstream::out);
  AST_ExpressionListIterator it;
  MAKE_SPACE;
  ret << "(";
  int size=out_expressions()->size(),i=0;
  foreach(it,out_expressions()) {
    i++;
    ret << current_element(it);
    ret << (i<size ? "," : "");
  }
  ret << "):=";
  ret << function();
  ret << "(";
  size=arguments()->size();
  i=0;
  foreach(it,arguments()) {
    i++;
    ret << current_element(it);
    ret << (i<size ? "," : "");
  }
  ret << ");" << endl;
  return ret.str();
}
