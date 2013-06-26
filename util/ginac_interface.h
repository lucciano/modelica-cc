
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

#ifndef GINAC_INTERFACE
#define GINAC_INTERFACE
#include <ast/ast_builder.h>
#include <util/ast_util.h>
#include <ginac/ginac.h>

DECLARE_FUNCTION_2P(var)
DECLARE_FUNCTION_1P(der)

class ConvertToGiNaC: public AST_Expression_Fold<GiNaC::ex> {
public:
  ConvertToGiNaC(VarSymbolTable  varEnv,bool forDerivation=false);
  GiNaC::ex convert(AST_Expression);
  GiNaC::symbol& getSymbol(AST_Expression_ComponentReference);
  GiNaC::symbol& getSymbol(AST_Expression_Derivative);
  GiNaC::symbol& getTime();
private:
  virtual GiNaC::ex foldTraverseElement(AST_Expression);
  virtual GiNaC::ex foldTraverseElementUMinus(AST_Expression);
  virtual GiNaC::ex foldTraverseElement(GiNaC::ex , GiNaC::ex , BinOpType);
  map<string, GiNaC::symbol> directory;
  bool _forDerivation;
  VarSymbolTable  _varEnv;
};

class ConvertToExpression {
public:
  static AST_Expression convert(GiNaC::ex);
};
#endif
