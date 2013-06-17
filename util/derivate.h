
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

#ifndef DERIVATE_H
#define DERIVATE_H

#include <ast/ast_builder.h>
#include <util/ast_util.h>
#include <ginac/ginac.h>

class EquationDerivator {
public:
  static AST_Equation_Equality derivate(AST_Equation_Equality eq,VarSymbolTable  varEnv);
};

#endif
