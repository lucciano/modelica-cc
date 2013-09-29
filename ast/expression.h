
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

#ifndef AST_EXPRESSION
#define AST_EXPRESSION

#include <sstream>
#include <ast/ast_node.h>
#include <ast/ast_builder.h>

/* Expression base class */
class AST_Expression_: public AST_Node_ {
public:
  DEFINE_CLASS_PRINTER(AST_Expression);
  virtual string print() const =0;
  virtual ExpressionType expressionType();

  /* Dynamic casts */
  GET_AS(Expression,BinOp);
  GET_AS(Expression,Boolean);
  GET_AS(Expression,BooleanNot);
  GET_AS(Expression,Brace);
  GET_AS(Expression,Call);
  GET_AS(Expression,CallArgs);
  GET_AS(Expression,ComponentReference);
  GET_AS(Expression,Derivative);
  GET_AS(Expression,If);
  GET_AS(Expression,Integer);
  GET_AS(Expression,Output);
  GET_AS(Expression,Range);
  GET_AS(Expression,Real);
  GET_AS(Expression,UMinus);
  GET_AS(Expression,String);
  AST_Expression_If_ElseIf getAsElseIf();
};

/* Integer */
class AST_Expression_Integer_: public AST_Expression_ {
public:
  AST_Expression_Integer_(AST_Integer i);
  string print() const;
  AST_Integer val();
  virtual ExpressionType expressionType();

private:
  AST_Integer _i;
};

/* Real */
class AST_Expression_Real_: public AST_Expression_ {
public:
  AST_Expression_Real_(double d);
  string print() const ;
  virtual ExpressionType expressionType();
  double val() {
    return _d;
  }

private:
  double _d;
};

/* String */
class AST_Expression_String_: public AST_Expression_ {
public:
  AST_Expression_String_(string s);
  string print() const;
  virtual ExpressionType expressionType();

private:
  string _s;
};

/* Boolean */
class AST_Expression_Boolean_: public AST_Expression_ {
public:
  AST_Expression_Boolean_(bool b);
  string print() const ;
  bool value();
  virtual ExpressionType expressionType();

private:
  bool _b;
};

/* Derivative */
class AST_Expression_Derivative_: public AST_Expression_ {
public:
  AST_Expression_Derivative_(AST_ExpressionList el);
  virtual ExpressionType expressionType();
  AST_ExpressionList arguments();
  string print() const;

private:
  AST_ExpressionList _el;
};

/* Unary minus */
class AST_Expression_UMinus_: public AST_Expression_ {
public:
  AST_Expression_UMinus_(AST_Expression e);
  string print() const ;
  AST_Expression exp() const;
  virtual ExpressionType expressionType();

private:
  AST_Expression _e;
};

/* Boolean not */
class AST_Expression_BooleanNot_: public AST_Expression_ {
public:
  AST_Expression_BooleanNot_(AST_Expression e);
  virtual ExpressionType expressionType();
  string print() const;
  AST_Expression exp() const;

private:
  AST_Expression _e;
};

/* Function call */
class AST_Expression_Call_: public AST_Expression_ {
public:
  AST_Expression_Call_(AST_String name, AST_ExpressionList args);
  string print() const;
  AST_String name() const;
  AST_ExpressionList arguments() const;
  virtual ExpressionType expressionType();

private:
  AST_String _name;
  AST_ExpressionList _args;
};

/* Call arguments */
class AST_Expression_CallArgs_: public AST_Expression_ {
public:
  AST_Expression_CallArgs_(AST_ExpressionList args);
  AST_ExpressionList arguments() const;
  virtual ExpressionType expressionType();
  string print() const ;

private:
  AST_ExpressionList _args;
};

/* Brace expression */
class AST_Expression_Brace_: public AST_Expression_ {
public:
  AST_Expression_Brace_(AST_ExpressionList args);
  AST_ExpressionList arguments() const;
  virtual ExpressionType expressionType();
  string print() const ;

private:
  AST_ExpressionList _args;
};

/* Component reference */
class AST_Expression_ComponentReference_: public AST_Expression_ {
public:
  AST_Expression_ComponentReference_(AST_String name, AST_ExpressionList);
  AST_Expression_ComponentReference_();
  string print() const;
  void append(AST_String,AST_ExpressionList);
  void prepend(AST_String,AST_ExpressionList);
  virtual ExpressionType expressionType();
  AST_StringList names() const;
  string name();
  AST_ExpressionListList indexes() const;

private:
  AST_StringList _name;
  AST_ExpressionListList _indexes;
};

/* Binary operation */
class AST_Expression_BinOp_: public AST_Expression_ {
public:
  AST_Expression_BinOp_(AST_Expression e1,AST_Expression e2, BinOpType t);
  virtual string print() const;
  virtual ExpressionType expressionType();
  AST_Expression left() const;
  AST_Expression right() const;
  BinOpType binopType() const;

private:
  AST_Expression _e1,_e2;
  BinOpType _t;
};

/* If expression */
class AST_Expression_If_: public AST_Expression_ {
public:
  AST_Expression_If_(AST_Expression cond, AST_Expression then, AST_Expression else_exp, AST_ExpressionList elseif_list);
  virtual string print() const;
  AST_Expression condition() const;
  AST_Expression then()  const;
  AST_Expression else_exp()  const;
  AST_ExpressionList elseif_list() const;
  virtual ExpressionType expressionType();

private:
  AST_Expression _cond, _then, _else_exp;
  AST_ExpressionList _elseif_list;
};

/* End expression (in subscripts) */
class AST_Expression_End_: public AST_Expression_ {
public:
  virtual string print() const ;
  virtual ExpressionType expressionType();
};

/* Null expression (for optional) */
class AST_Expression_Null_: public AST_Expression_ {
public:
  virtual string print() const;
  virtual ExpressionType expressionType();
};

/* Else If as an expression */
class AST_Expression_If_ElseIf_: public AST_Expression_ {
public:
  AST_Expression_If_ElseIf_(AST_Expression,AST_Expression);
  virtual string print() const;
  virtual ExpressionType expressionType();
  AST_Expression condition();
  AST_Expression then();

private:
  AST_Expression _cond;
  AST_Expression _then;
} ;

/* Colon expression for subscripts */
class AST_Expression_Colon_: public AST_Expression_ {
public:
  virtual string print() const;
  virtual ExpressionType expressionType();
};

/* Output expressions like (a,b,c)=f();) */
class AST_Expression_Output_: public AST_Expression_ {
public:
  AST_Expression_Output_(AST_ExpressionList);

  virtual ExpressionType expressionType();
  virtual string print() const;
  AST_ExpressionList expressionList();

private:
  AST_ExpressionList _list;
};

/* Range expression ( 1:2:4 ) */
class AST_Expression_Range_: public AST_Expression_ {
public:
  AST_Expression_Range_(AST_ExpressionList);

  virtual ExpressionType expressionType();
  virtual string print() const;
  AST_ExpressionList expressionList();

private:
  AST_ExpressionList _list;
};

#endif
