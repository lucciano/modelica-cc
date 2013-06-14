
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
#include <util/ginac_interface.h>

AST_Expression EquationSolver::solve(AST_Equation_Equality eq, AST_Expression_ComponentReference cr) {
  ConvertToGiNaC tog(NULL); // No var symbol table needed for now
  ConvertToExpression toe;
  GiNaC::ex left=tog.convert(eq->left());
  GiNaC::ex right=tog.convert(eq->right());
  if (!left.has(tog.getSymbol(cr)) && !right.has(tog.getSymbol(cr))) {
    cerr << "SOLVE: Variable " << tog.getSymbol(cr) << " not found in equation"<<endl;
    return newAST_Expression_Null();
  }
  GiNaC::ex res= lsolve(left==right, tog.getSymbol(cr));
  return toe.convert(res);
}

