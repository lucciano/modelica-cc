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
  friend ostream & operator<<(ostream &os , const AST_Equation_ &e ){
    os << e.print();  
    return os;
  };
  virtual string print() const =0;
  virtual EquationType equationType() = 0;
  virtual AST_Equation duplicateEquation();
  AST_Equation_Equality getAsEquality();
  AST_Equation_Connect getAsConnect();
};

class AST_Equation_Equality_: public AST_Equation_
{
public:
  AST_Equation_Equality_(AST_Expression left, AST_Expression right):_left(left), _right(right) {};
  virtual EquationType equationType() { return EQEQUALITY; }
  string print() const { 
    stringstream ret(stringstream::out);
    ret << left() << " = " << right() << ";" << endl;
    return ret.str();
  };

  AST_Expression _left,_right;
  AST_Expression left() const;
  AST_Expression right() const;
};

class AST_Equation_Connect_: public AST_Equation_ {
public:
  AST_Equation_Connect_(AST_Expression_ComponentReference cr1, AST_Expression_ComponentReference cr2):_cr1(cr1), _cr2(cr2) {};

  string print() const { 
    stringstream ret(stringstream::out);
    ret << "CONNECT[" << _cr1 << "," << _cr2 <<"]" << endl;
    return ret.str();
  }
  AST_Expression_ComponentReference _cr1, _cr2;
  AST_Expression_ComponentReference left() { return _cr1; }
  AST_Expression_ComponentReference right() { return _cr2; }
  virtual EquationType equationType() { return EQCONNECT; }
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
