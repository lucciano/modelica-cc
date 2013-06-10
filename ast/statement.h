
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

#ifndef AST_STATMENT
#define AST_STATMENT
#include <ast/ast_node.h>
#include <ast/ast_builder.h>

class AST_Statement_: public AST_Node_
{
public:
  DEFINE_CLASS_PRINTER(AST_Statement);
  virtual string print() const =0;
  virtual StatementType statementType() = 0;

  /* Dynamic casts */
  GET_AS(Statement,While);
  GET_AS(Statement,If);
  GET_AS(Statement,For);
  GET_AS(Statement,When);
  GET_AS(Statement,Assign);
};


class AST_Statement_Return_: public AST_Statement_ {
  string print() const;
  virtual StatementType statementType();
};

class AST_Statement_Break_: public AST_Statement_ {
  string print() const;
  virtual StatementType statementType();
};

class AST_Statement_When_: public AST_Statement_ {
public:
  AST_Statement_When_(AST_Expression cond, AST_StatementList,AST_Statement_ElseList);
  string print() const;
  AST_Expression condition() const;
  AST_StatementList statements() const;
  AST_Statement_ElseList else_when() const;
  virtual StatementType statementType();

private:
  AST_Expression _cond;
  AST_StatementList _sts;
  AST_Statement_ElseList _else_list;
};

class AST_Statement_Assign_: public AST_Statement_ {
public:
  AST_Statement_Assign_(AST_Expression_ComponentReference cr, AST_Expression exp);
  string print() const;
  AST_Expression exp() const;
  AST_Expression_ComponentReference lhs() const;
  virtual StatementType statementType();

private:
  AST_Expression_ComponentReference _lhs;
  AST_Expression _exp;
};

class AST_Statement_If_: public AST_Statement_ {
public:
  AST_Statement_If_(AST_Expression cond, AST_StatementList true_st, AST_Statement_ElseList, AST_StatementList false_st);
  string print() const;
  AST_Expression condition() const;
  AST_StatementList statements() const;
  AST_StatementList else_statements()const;
  AST_Statement_ElseList else_if() const;
  virtual StatementType statementType();

private:
  AST_Expression _cond;
  AST_StatementList _true_st, _false_st;
  AST_Statement_ElseList _else_list;
};

/* While */
class AST_Statement_While_: public AST_Statement_ {
public:
  AST_Statement_While_(AST_Expression cond, AST_StatementList);
  string print() const;
  AST_Expression condition() const;
  AST_StatementList statements() const;
  virtual StatementType statementType();

private:
  AST_Expression _cond;
  AST_StatementList _sts;
};

class AST_Statement_For_: public AST_Statement_ {
public:
  AST_Statement_For_(AST_ForIndexList index, AST_StatementList);
  string print() const;
  AST_StatementList statements() const;
  AST_ForIndexList forIndexList() const;
  virtual StatementType statementType();

private:
  AST_StatementList _sts;
  AST_ForIndexList _ind;
};

class AST_Statement_OutputAssigment_: public AST_Statement_ {
public:
  AST_Statement_OutputAssigment_(AST_ExpressionList, AST_Expression_ComponentReference, AST_ExpressionList);
  string print() const;
  AST_ExpressionList out_expressions() const;
  AST_Expression_ComponentReference function() const;
  AST_ExpressionList arguments() const;
  virtual StatementType statementType();

private:
  AST_ExpressionList _out_exps;
  AST_Expression_ComponentReference _func;
  AST_ExpressionList _args;
};

class AST_Statement_Else_: public AST_Node_ {
public:
  AST_Statement_Else_ (AST_Expression cond, AST_StatementList sts);
  AST_Expression condition() const;
  AST_StatementList statements() const;

private:
  AST_Expression _cond;
  AST_StatementList  _sts;
};


#endif
