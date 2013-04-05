
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

class AST_Expression_: public AST_Node {
public:
  friend ostream & operator<<(ostream &os , const AST_Expression_ &e );
  virtual string print() const =0;
  virtual ExpressionType expressionType();
  AST_Expression_ComponentReference getAsComponentRef();
  AST_Expression_BinOp getAsBinOp();
  AST_Expression_Derivative getAsDerivative(); 
  virtual AST_Expression duplicateExpression();
};

class AST_Expression_Integer_: public AST_Expression_ {
public:
  AST_Expression_Integer_(AST_Integer i);
  string print() const;
  virtual AST_Expression duplicateExpression();
  AST_Integer val();
private:
  AST_Integer _i;
};

class AST_Expression_Real_: public AST_Expression_ {
public:
  AST_Expression_Real_(double d);
  string print() const ;
  double val();
private:

  double _d;
};

class AST_Expression_String_: public AST_Expression_ {
public:
  AST_Expression_String_(string s);
  string print() const;

private:
  string _s;
};

class AST_Expression_Boolean_: public AST_Expression_ {
public:
  AST_Expression_Boolean_(bool b);
  string print() const ;
  bool value();

private:
  bool _b;
};

class AST_Expression_Derivative_: public AST_Expression_ {
public:
  AST_Expression_Derivative_(AST_ExpressionList el);
  virtual ExpressionType expressionType() { return EXPDERIVATIVE; }
  virtual AST_Expression duplicateExpression();
  AST_ExpressionList arguments() { return _el; }
  string print() const;

private:
  AST_ExpressionList _el;
};

class AST_Expression_UMinus_: public AST_Expression_ {
public:
  AST_Expression_UMinus_(AST_Expression e);
  string print() const ;
  AST_Expression exp() const { return _e; }
private:
  AST_Expression _e;
};

class AST_Expression_BooleanNot_: public AST_Expression_ {
public:
  AST_Expression_BooleanNot_(AST_Expression e);
  string print() const;
  AST_Expression exp() const { return _e; }

private:
  AST_Expression _e;
};

class AST_Expression_Call_: public AST_Expression_ {
public:
  AST_Expression_Call_(AST_String name);
  string print() const ;
  AST_String name() const { return _name; }
  virtual ExpressionType expressionType();

private:
  AST_String _name;
};

class AST_Expression_ComponentReference_: public AST_Expression_ {
public:
  AST_Expression_ComponentReference_(string name);
  AST_Expression_ComponentReference_(string name, AST_ExpressionList);
  AST_Expression_ComponentReference_();
  string print() const;
  void append(AST_Expression_ComponentReference cr);
  void append(string s);
  void append(string s,AST_ExpressionList);
  void setName(string name);
  string name() const { return _name; }
  virtual ExpressionType expressionType() { return EXPCOMPREF; }
  virtual AST_Expression duplicateExpression();

private:
  string _name;
};

class AST_Expression_BinOp_: public AST_Expression_ {
public:
  AST_Expression_BinOp_(AST_Expression e1,AST_Expression e2, BinOpType t);
  virtual string print() const;
  AST_Expression left() const { return _e1; }
  AST_Expression right() const { return _e2; }
  virtual ExpressionType expressionType() { return EXPBINOP; }
  BinOpType binopType() const { return _t;}
private:
  BinOpType _t;
  AST_Expression _e1,_e2;
};

class AST_Expression_If_: public AST_Expression_ {
public:
  AST_Expression_If_(AST_Expression cond, AST_Expression then, AST_Expression else_exp);
  virtual string print() const;
  AST_Expression condition() const { return _cond; }
  AST_Expression then()  const{ return _then; }
  AST_Expression else_exp()  const{ return _else_exp; }
  virtual ExpressionType expressionType();
private:
  AST_Expression _cond, _then, _else_exp;
};

class AST_Expression_End_: public AST_Expression_ {
public:
  virtual string print() const ;//{ return "end";};
  virtual ExpressionType expressionType() { return EXPEND; }
};

class AST_Expression_Null_: public AST_Expression_ {
public:
  virtual string print() const {};
  virtual ExpressionType expressionType() { return EXPNULL; }
};

class AST_Expression_If_ElseIf_: public AST_Expression_ {
public:
  AST_Expression_If_ElseIf_(AST_Expression,AST_Expression);
  virtual string print() const {};
  virtual ExpressionType expressionType() { return EXPELSEIF; }
private:
  AST_Expression _cond;
  AST_Expression _then;
} ;
class AST_Expression_Colon_: public AST_Expression_ {
public:
  virtual string print() const { return ":";};
  virtual ExpressionType expressionType() { return EXPCOLON; }
};


#endif
