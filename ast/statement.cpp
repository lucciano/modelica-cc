
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

#include <ast/statement.h>
#include <iostream>
#include <sstream>


using namespace std;

ostream & operator<<(ostream &os , const AST_Statement_ &s ) {
  os << s.print();  
  return os;
}

string AST_Statement_Return_::print() const { 
  return "return"; 
}

string AST_Statement_Break_::print() const { 
  return "return"; 
}

string AST_Statement_Assign_::print() const { 
  stringstream ret(stringstream::out);
  ret << lhs() << ":=" << exp() << ";"<<endl;
  return ret.str();
}
AST_Statement_Assign_::AST_Statement_Assign_(AST_Expression_ComponentReference lhs,AST_Expression exp): _lhs(lhs), _exp(exp) {
}


string AST_Statement_When_::print() const { 
  stringstream ret(stringstream::out);
  AST_StatementListIterator it;
  ret << "when " << condition() << " then"<<endl;
  foreach(it,statements())
    ret << current(it);
  ret << "  end when;" <<endl;
  return ret.str();
}
AST_Statement_When_::AST_Statement_When_(AST_Expression cond, AST_StatementList sts): _cond(cond), _sts(sts) {
}

ostream & operator<<(ostream &os , const AST_Statement &s ) {
  os << *s;
  return os;
}
