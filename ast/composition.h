
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
#ifndef AST_COMPOSITION
#define AST_COMPOSITION

#include <ast/ast_node.h>
#include <ast/element.h>
#include <ast/equation.h>
#include <stdlib.h>

class AST_CompositionElement_;

class AST_Composition_: public AST_Node_ {
public:
  AST_Composition_ (AST_ElementList el,AST_CompositionElementList cl);
  DEFINE_CLASS_PRINTER(AST_Composition);
  AST_CompositionElementList compositionList() const;
  AST_ElementList elementList() const;

private:
  AST_CompositionElementList _composition_list;
  AST_ElementList _element_list;
};

class AST_CompositionElement_: public AST_Node_
{
public:
  AST_CompositionElement_(AST_CompositionEqsAlgs eqs_algs);
  AST_CompositionElement_(AST_ElementList el);
  DEFINE_CLASS_PRINTER(AST_CompositionElement);
  AST_CompositionEqsAlgs getEquationsAlgs();
  AST_ElementList getElementList();

private:
  AST_CompositionEqsAlgs _eqs_algs;
  AST_ElementList _el;
};


class AST_CompositionEqsAlgs_: public AST_Node_ {
public:
  AST_CompositionEqsAlgs_(AST_EquationList eq);
  AST_CompositionEqsAlgs_(AST_StatementList st);
  AST_CompositionEqsAlgs_(AST_StatementList st, bool initial);
  AST_CompositionEqsAlgs_(AST_EquationList eq, bool initial);
  AST_EquationList getEquations();
  AST_StatementList getAlgorithms();
  bool isInitial();

private:
  bool _initial;
  AST_EquationList _eq;
  AST_StatementList _st;
};
#endif
