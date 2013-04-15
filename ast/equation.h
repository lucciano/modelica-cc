
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
};

class AST_Equation_Equality_: public AST_Equation_
{
public:
  AST_Equation_Equality_(AST_Expression left, AST_Expression right);
  virtual EquationType equationType() { return EQEQUALITY; }
  string print() const ;
  AST_Expression left() const;
  AST_Expression right() const;

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
  AST_Equation_If_(AST_Expression cond, AST_EquationList eql): _cond(cond), _eql(eql) {};
  string print() const;
  AST_Expression condition() const { return _cond; };
  AST_EquationList equationList() const { return _eql; };
  virtual EquationType equationType() { return EQIF; }

private:
  AST_Expression _cond;
  AST_EquationList _eql;
};

class AST_Equation_For_: public AST_Equation_ {
public:
  AST_Equation_For_(AST_ForIndexList ind,AST_EquationList eql):  _eql(eql) {};
  string print() const;
  AST_EquationList equationList() const { return _eql; };
  virtual EquationType equationType() { return EQFOR; }

private:
  AST_EquationList _eql;
};

class AST_ForIndex_: public AST_Node {
  AST_ForIndex_(AST_String, AST_Expression) {}
  
private:
  AST_String var;
  AST_Expression in;
};

class AST_Equation_Else_: public AST_Node {
public:
  AST_Equation_Else_ (AST_Expression, AST_EquationList) {}
  
private:
  AST_Expression _cond;
  AST_EquationList _eqs;
};
#endif
