
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

REGISTER_FUNCTION(der, dummy())

ConvertToGiNaC::ConvertToGiNaC(VarSymbolTable  varEnv, bool forDerivation): _varEnv(varEnv),_forDerivation(forDerivation) {}

GiNaC::ex ConvertToGiNaC::convert(AST_Expression e) {
  return foldTraverse(e);
}


static GiNaC::ex var_derivative(const GiNaC::ex & x,const GiNaC::ex & y, unsigned diff_param) {
  return der(x);
}
REGISTER_FUNCTION(var, derivative_func(var_derivative))

GiNaC::ex ConvertToGiNaC::foldTraverseElement(GiNaC::ex l, GiNaC::ex r, BinOpType b) {
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

GiNaC::symbol& ConvertToGiNaC::getSymbol(AST_Expression_Derivative der) {
  AST_Expression_ComponentReference cr=der->arguments()->front()->getAsComponentReference();
  string s=cr->name();
  s.insert(0,"__der_");
  AST_Expression_ComponentReference new_cr=AST_Expression_ComponentReference_Add(newAST_Expression_ComponentReference(),newAST_String(s),newAST_ExpressionList());
  return getSymbol(new_cr);
}
GiNaC::symbol& ConvertToGiNaC::getSymbol(AST_Expression_ComponentReference cr) {
  string s=cr->name();
  map<string, GiNaC::symbol>::iterator i = directory.find(s);
  if (i != directory.end())
    return i->second;
  else
    return directory.insert(make_pair(s, GiNaC::symbol(s))).first->second;
}

GiNaC::symbol& ConvertToGiNaC::getTime() {
  string s="time";
  map<string, GiNaC::symbol>::iterator i = directory.find(s);
  if (i != directory.end())
    return i->second;
  return directory.insert(make_pair(s, GiNaC::symbol(s))).first->second;
}

GiNaC::ex ConvertToGiNaC::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
  case EXPREAL:
    return GiNaC::ex(e->getAsReal()->val());
  case EXPINTEGER:
    return GiNaC::ex(e->getAsInteger()->val());
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
  default:
    cerr << "Expression: " << e << " not converted to GiNaC" << endl;
    return GiNaC::ex(0);
  }
}

AST_Expression ConvertToExpression::convert(GiNaC::ex exp) {
  stringstream s(ios_base::out);
  int r;
  s << exp;
  AST_Expression e= parseExpression(s.str().c_str(),&r);
  assert(e!=NULL && r==0) ;
  return e;

}
