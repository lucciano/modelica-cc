
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

#include <util/solve.h>
#include <util/derivate.h>
#include <util/ginac_interface.h>
#include <parser/parse.h>
#include <ast/equation.h>


AST_Equation_Equality EquationDerivator::derivate(AST_Equation_Equality eq,VarSymbolTable  varEnv) {
  ConvertToGiNaC tog(varEnv,true);
  ConvertToExpression toe;
  GiNaC::ex left      = tog.convert(eq->left());
  GiNaC::ex right     = tog.convert(eq->right());
  GiNaC::symbol time  = tog.getTime();
  GiNaC::ex der_left  = left.diff(time).subs(var(GiNaC::wild(),time)==GiNaC::wild()) ;
  GiNaC::ex der_right = right.diff(time).subs(var(GiNaC::wild(),time)==GiNaC::wild()) ;
  return newAST_Equation_Equality(toe.convert(der_left),toe.convert(der_right))->getAsEquality();
}
