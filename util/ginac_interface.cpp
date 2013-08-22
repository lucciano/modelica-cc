
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
#include <util/solve.h>
#include <parser/parse.h>
#include <ast/equation.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sstream>
#include <ginac/ginac.h>
#include <util/ginac_interface.h>

using namespace GiNaC;
REGISTER_FUNCTION(der, dummy())
void my_print_power_dflt(const power & p, const print_dflt & c, unsigned level) {
  // get the precedence of the 'power' class
  unsigned power_prec = p.precedence();

  // if the parent operator has the same or a higher precedence
  // we need parentheses around the power
  if (level >= power_prec)
    c.s << '(';
  if (p.op(1).match(-1)) {
    c.s << "(1/" << p.op(0) << ")";
  } else {
    c.s << p.op(0) << "^" << p.op(1);
  }
  // don't forget the closing parenthesis
  if (level >= power_prec)
    c.s << ')';
}

void my_print_mul_dflt(const mul & m, const print_dflt & c, unsigned level) {
  // get the precedence of the 'power' class
  unsigned power_prec = m.precedence();
  if (level >= power_prec)
    c.s << '(';
  if (m.op(1).match(pow(wild(),-1))) {
    c.s << "(" << m.op(0) << "/" << m.op(1).op(0) << ")";
  } else if (m.op(0).match(pow(wild(),-1))) {
    c.s << "(" << m.op(1) << "/" << m.op(0).op(0) << ")";
  } else {
    c.s << m.op(0) << "*" << m.op(1);
  }
  if (level >= power_prec)
    c.s << ')';
}
void my_print_add_dflt(const add & s, const print_dflt & c, unsigned level) {
  // get the precedence of the 'power' class
  unsigned power_prec = s.precedence();
  if (level >= power_prec)
    c.s << '(';
  if (s.op(0).match(-wild()) && !s.op(1).match(-wild())) {
    c.s << s.op(1) << "-" << s.op(0).op(0);
  } else {
    c.s << s.op(0) << "+" << s.op(1);
  }
  if (level >= power_prec)
    c.s << ')';
}

ConvertToGiNaC::ConvertToGiNaC(VarSymbolTable  varEnv, bool forDerivation): _varEnv(varEnv),_forDerivation(forDerivation) {}

ex ConvertToGiNaC::convert(AST_Expression e) {
  return foldTraverse(e);
}


static ex var_derivative(const ex & x,const ex & y, unsigned diff_param) {
  return der(x);
}
REGISTER_FUNCTION(var, derivative_func(var_derivative))

ex ConvertToGiNaC::foldTraverseElement(ex l, ex r, BinOpType b) {
  switch (b) {
  case BINOPADD:
    return l+r;
  case BINOPSUB:
    return l-r;
  case BINOPMULT:
    return l*r;
  case BINOPDIV:
    return l/r;
  }
}

symbol& ConvertToGiNaC::getSymbol(AST_Expression_Derivative der) {
  AST_Expression_ComponentReference cr=der->arguments()->front()->getAsComponentReference();
  string s=cr->name();
  s.insert(0,"__der_");
  AST_Expression_ComponentReference new_cr=AST_Expression_ComponentReference_Add(newAST_Expression_ComponentReference(),newAST_String(s),newAST_ExpressionList());
  return getSymbol(new_cr);
}
symbol& ConvertToGiNaC::getSymbol(AST_Expression_ComponentReference cr) {
  string s=cr->name();
  map<string, symbol>::iterator i = directory.find(s);
  if (i != directory.end())
    return i->second;
  else
    return directory.insert(make_pair(s, symbol(s))).first->second;
}

symbol& ConvertToGiNaC::getTime() {
  string s="time";
  map<string, symbol>::iterator i = directory.find(s);
  if (i != directory.end())
    return i->second;
  return directory.insert(make_pair(s, symbol(s))).first->second;
}

ex ConvertToGiNaC::foldTraverseElementUMinus(AST_Expression e) {
  return -convert(e->getAsUMinus()->exp());
}
ex ConvertToGiNaC::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
  case EXPREAL:
    return ex(e->getAsReal()->val());
  case EXPINTEGER:
    return ex(e->getAsInteger()->val());
  case EXPCOMPREF: {
    if (!_forDerivation)
      return getSymbol(e->getAsComponentReference());
    else {
      VarInfo v = _varEnv->lookup(e->getAsComponentReference()->name());
      if (v->isParameter() || v->isDiscrete() || v->isConstant())
        return getSymbol(e->getAsComponentReference());
      else
        return var(getSymbol(e->getAsComponentReference()),getTime());
    }
  }
  case EXPDERIVATIVE:
    return getSymbol(e->getAsDerivative());
  /*case EXPUMINUS:
    return -convert(e->getAsUMinus()->exp());*/
  case EXPCALL:
  {
    AST_Expression_Call c=e->getAsCall();
    if (*c->name()=="sin") {
      return sin(convert(AST_ListFirst(c->arguments())));
    } else {
      cerr << "Function call : " << c->name() << " not converted to GiNaC" << endl;
      return ex(0);
    }
  }
  default:
    cerr << "Expression: " << e << " not converted to GiNaC" << endl;
    return ex(0);
  }
}

AST_Expression ConvertToExpression::convert(ex exp) {
  stringstream s(ios_base::out),der_s(ios_base::out);
  int r;
  set_print_func<power,print_dflt>(my_print_power_dflt);
  //set_print_func<mul,print_dflt>(my_print_mul_dflt);
  //set_print_func<add,print_dflt>(my_print_add_dflt);
  s << exp;
  AST_Expression e;
  if (s.str().find("__der_")==0) {
    string ss=s.str().erase(0,6);
    der_s << "der(" << ss << ")";
    e= parseExpression(der_s.str().c_str(),&r);
  } else {
    e= parseExpression(s.str().c_str(),&r);
  }
  assert(e!=NULL && r==0) ;
  return e;

}
