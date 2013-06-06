
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
#include <stdio.h>
#include <sstream>

#ifdef MATHOMATIC
extern "C" {
#include <mathomatic.h>
}

AST_Equation_Equality EquationSolver::solve(AST_Equation_Equality eq, AST_Expression_ComponentReference cr) {
  stringstream eqs(ios_base::out);
  stringstream crs(ios_base::out);
  int r;

  cerr << "Solving for " << cr << " from " << eq->print();
  eqs << eq;
  crs << "solve " << cr;
  matho_init();
  char *eq_buff= new char[eqs.str().size()];
  char *cr_buff= new char[crs.str().size()];
  strcpy(eq_buff,eqs.str().c_str());
  strcpy(cr_buff,crs.str().c_str());
  char *out;
  matho_process(eq_buff,&out);
  matho_process(cr_buff,&out);
  printf("Library result string:\n%s\n", out);
  string res(out+4);
  AST_Expression e= parseExpression(res,&r);
  cerr << "Result " << e << endl;
  clear_all();
  delete eq_buff;
  delete cr_buff;
  return NULL;
}

#else
// GiNaC

#include <ginac/ginac.h>
AST_Expression EquationSolver::solve(AST_Equation_Equality eq, AST_Expression_ComponentReference cr) {
  ConvertToGiNaC tog;
  stringstream eqs(ios_base::out);
  int r;

  GiNaC::ex left=tog.foldTraverse(eq->left());
  GiNaC::ex right=tog.foldTraverse(eq->right());
  GiNaC::ex res= lsolve(left==right, tog.getSymbol(cr));
  eqs << res;
  AST_Expression rhs=parseExpression(eqs.str().c_str(),&r);
  return rhs;
}


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

GiNaC::symbol& ConvertToGiNaC::getSymbol(AST_Expression_ComponentReference cr) {
  string s=cr->name();
  map<string, GiNaC::symbol>::iterator i = directory.find(s);
  if (i != directory.end())
    return i->second;
  else
  return directory.insert(make_pair(s, GiNaC::symbol(s))).first->second;
}

GiNaC::ex ConvertToGiNaC::foldTraverseElement(AST_Expression e) {
  switch (e->expressionType()) {
    case EXPREAL: 
     return GiNaC::ex(e->getAsReal()->val());
     break;
    case EXPINTEGER: 
     return GiNaC::ex(e->getAsInteger()->val());
     break;
    case EXPCOMPREF: {
     return getSymbol(e->getAsComponentReference());
    }
     break;
  }
}
#endif
