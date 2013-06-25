
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
#include <cassert>
#include <util/ginac_interface.h>

MMO_EquationList EquationSolver::solve(MMO_EquationList eqs, AST_ExpressionList crs) {
  ConvertToGiNaC tog(NULL); // No var symbol table needed for now
  ConvertToExpression toe;
  assert(AST_Length(eqs)==1); 
  assert(AST_Length(crs)==1); 
  AST_Equation_Equality eq=AST_ListFirst(eqs)->getAsEquality();
  assert(eq!=NULL); 

  GiNaC::ex left=tog.convert(eq->left());
  GiNaC::ex right=tog.convert(eq->right());
  AST_Expression cr_exp=AST_ListFirst(crs);
  if (cr_exp->expressionType()==EXPCOMPREF) {
    AST_Expression_ComponentReference cr=cr_exp->getAsComponentReference();
    if (!left.has(tog.getSymbol(cr)) && !right.has(tog.getSymbol(cr))) {
      cerr << "SOLVE: Variable " << tog.getSymbol(cr) << " not found in equation"<<endl;
      return newAST_EquationList();
    }
    GiNaC::ex rhs= lsolve(left==right, tog.getSymbol(cr));
    AST_Equation res=newAST_Equation_Equality(cr,toe.convert(rhs));
    return newAST_SimpleList(res);
  } else if (cr_exp->expressionType()==EXPDERIVATIVE) {
    AST_Expression_Derivative der=cr_exp->getAsDerivative();
    if (!left.has(tog.getSymbol(der)) && !right.has(tog.getSymbol(der))) {
      cerr << "SOLVE: Derivative " << tog.getSymbol(der) << " not found in equation"<<endl;
      return newAST_EquationList();
    }
    GiNaC::ex rhs= lsolve(left==right, tog.getSymbol(der));
    AST_Equation res=newAST_Equation_Equality(der,toe.convert(rhs));
    return newAST_SimpleList(res);
  }
}

