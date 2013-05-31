
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
class AST_Equation_: public AST_Node
{
public:
  friend ostream & operator<<(ostream &os , const AST_Equation_ &e );
  virtual string print() const =0;
  virtual EquationType equationType() = 0;
  AST_Equation_Equality getAsEquality();
  AST_Equation_Connect getAsConnect();
  AST_Equation_When getAsWhen();
  AST_Equation_If getAsIf();
  AST_Equation_For getAsFor();
};

class AST_Equation_Equality_: public AST_Equation_
{
public:
  AST_Equation_Equality_(AST_Expression left, AST_Expression right);
  virtual EquationType equationType() { return EQEQUALITY; }
  string print() const ;
  AST_Expression left() const;
  AST_Expression right() const;
  void setLeft(AST_Expression);
  void setRight(AST_Expression);

private:
  AST_Expression _left,_right;
};

class AST_Equation_Connect_: public AST_Equation_ {
public:
  AST_Equation_Connect_(AST_Expression_ComponentReference cr1, AST_Expression_ComponentReference cr2);

  string print() const ;
  AST_Expression_ComponentReference left() { return _cr1; }
  AST_Expression_ComponentReference right() { return _cr2; }
  virtual EquationType equationType() { return EQCONNECT; }

private:
  AST_Expression_ComponentReference _cr1, _cr2;
};

class AST_Equation_Call_: public AST_Equation_ {
public:
  AST_Equation_Call_(AST_Expression e);
  string print() const;
  virtual EquationType equationType() { return EQCONNECT; }
  AST_Expression call() const;

private:
  AST_Expression _call;
}; 

class AST_Equation_If_: public AST_Equation_ {
public:
  AST_Equation_If_(AST_Expression cond, AST_EquationList eql, AST_Equation_ElseList elseif, AST_EquationList else_eq): 
      _cond(cond), _eql(eql),_else(else_eq),_else_if(elseif) {};
  string print() const;
  AST_Expression condition() const { return _cond; };
  AST_EquationList equationList() const { return _eql; };
  AST_EquationList equationElseList() const { return _else; };
  AST_Equation_ElseList equationElseIf() const { return _else_if;}
  virtual EquationType equationType() { return EQIF; }

private:
  AST_Expression _cond;
  AST_EquationList _eql;
  AST_EquationList _else;
  AST_Equation_ElseList _else_if;
};

class AST_Equation_For_: public AST_Equation_ {
public:
  AST_Equation_For_(AST_ForIndexList ind,AST_EquationList eql):  _eql(eql) , _ind(ind) {};
  string print() const;
  AST_EquationList equationList() const { return _eql; };
  AST_ForIndexList forIndexList() const { return _ind; };
  virtual EquationType equationType() { return EQFOR; }

private:
  AST_EquationList _eql;
  AST_ForIndexList _ind;
};

class AST_ForIndex_: public AST_Node {
public:
  AST_ForIndex_(AST_String var, AST_Expression exp):_var(var),_in(exp) {}
  string print() const;
  AST_String variable() const { return _var; }
  AST_Expression in_exp() const { return _in; }
  friend ostream & operator<<(ostream &os , const AST_ForIndex_ &f );
  
private:
  AST_String _var;
  AST_Expression _in;
};

class AST_Equation_Else_: public AST_Node {
public:
  AST_Equation_Else_ (AST_Expression cond, AST_EquationList eqs): _cond(cond), _eqs(eqs) {}
  AST_Expression condition() const { return _cond; }
  AST_EquationList equations() const { return _eqs; }
  
private:
  AST_Expression _cond;
  AST_EquationList _eqs;
};

class AST_Equation_When_: public AST_Equation_ {
public:
  AST_Equation_When_(AST_Expression cond, AST_EquationList eql, AST_Equation_ElseList elsewhen): _cond(cond), _eql(eql),_else_when(elsewhen) {};
  string print() const;
  AST_Expression condition() const { return _cond; };
  AST_EquationList equationList() const { return _eql; };
  virtual EquationType equationType() { return EQWHEN; }
  AST_Equation_ElseList equationElseWhen() const { return _else_when;}
  void setCondition(AST_Expression e) { _cond=e; }

private:
  AST_Expression _cond;
  AST_EquationList _eql;
  AST_Equation_ElseList _else_when;
};


#endif

