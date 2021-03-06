
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

#include <string>
#include <list>
#include <ast/ast_node.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>

#ifndef AST_EQUATION
#define AST_EQUATION

/* Base class for equations */
class AST_Equation_: public AST_Node_
{
public:
  DEFINE_CLASS_PRINTER(AST_Equation);
  virtual string print() const =0;
  virtual EquationType equationType() = 0;

  /* Dynamic casts */
  GET_AS(Equation,Call);
  GET_AS(Equation,Connect);
  GET_AS(Equation,Equality);
  GET_AS(Equation,For);
  GET_AS(Equation,If);
  GET_AS(Equation,When);
};

/* Equality */
class AST_Equation_Equality_: public AST_Equation_
{
public:
  AST_Equation_Equality_(AST_Expression left, AST_Expression right);
  virtual EquationType equationType();
  string print() const ;
  AST_Expression left() const;
  AST_Expression right() const;
  void setLeft(AST_Expression);
  void setRight(AST_Expression);

private:
  AST_Expression _left,_right;
};

/* Connect equation */
class AST_Equation_Connect_: public AST_Equation_ {
public:
  AST_Equation_Connect_(AST_Expression_ComponentReference cr1, AST_Expression_ComponentReference cr2);

  string print() const ;
  AST_Expression_ComponentReference left();
  AST_Expression_ComponentReference right();
  virtual EquationType equationType();

private:
  AST_Expression_ComponentReference _cr1, _cr2;
};

/* Call equaiton */
class AST_Equation_Call_: public AST_Equation_ {
public:
  AST_Equation_Call_(AST_Expression e);
  string print() const;
  virtual EquationType equationType();
  AST_Expression call() const;

private:
  AST_Expression _call;
};

/* If equation */
class AST_Equation_If_: public AST_Equation_ {
public:
  AST_Equation_If_(AST_Expression cond, AST_EquationList eql, AST_Equation_ElseList elseif, AST_EquationList else_eq);
  string print() const;
  AST_Expression condition() const;
  void setCondition(AST_Expression c);
  AST_EquationList equationList() const;
  AST_EquationList equationElseList() const;
  AST_Equation_ElseList equationElseIf() const;
  virtual EquationType equationType();

private:
  AST_Expression _cond;
  AST_EquationList _eql;
  AST_EquationList _else;
  AST_Equation_ElseList _else_if;
};

/* For equation */
class AST_Equation_For_: public AST_Equation_ {
public:
  AST_Equation_For_(AST_ForIndexList ind,AST_EquationList eql);
  string print() const;
  AST_EquationList equationList() const;
  AST_ForIndexList forIndexList() const;
  virtual EquationType equationType();

private:
  AST_EquationList _eql;
  AST_ForIndexList _ind;
};

/* For index */
class AST_ForIndex_: public AST_Node_ {
public:
  AST_ForIndex_(AST_String var, AST_Expression exp);
  DEFINE_CLASS_PRINTER(AST_ForIndex);
  string print() const;
  AST_String variable() const;
  AST_Expression in_exp() const;

private:
  AST_String _var;
  AST_Expression _in;
};

/* Equation else */
class AST_Equation_Else_: public AST_Node_ {
public:
  AST_Equation_Else_ (AST_Expression cond, AST_EquationList eqs);
  AST_Expression condition() const;
  void setCondition(AST_Expression e);
  AST_EquationList equations() const;

private:
  AST_Expression _cond;
  AST_EquationList _eqs;
};

/* When Equation */
class AST_Equation_When_: public AST_Equation_ {
public:
  AST_Equation_When_(AST_Expression cond, AST_EquationList eql, AST_Equation_ElseList elsewhen);
  string print() const;
  AST_Expression condition() const;
  AST_EquationList equationList() const;
  virtual EquationType equationType();
  AST_Equation_ElseList equationElseWhen() const;
  void setCondition(AST_Expression e);

private:
  AST_Expression _cond;
  AST_EquationList _eql;
  AST_Equation_ElseList _else_when;
};
#endif

