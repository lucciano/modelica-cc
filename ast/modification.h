
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

#ifndef AST_MODIFICAITON
#define AST_MODIFICAITON

#include <ast/ast_node.h>
#include <ast/ast_builder.h>
#include <sstream>

class AST_Argument_: public AST_Node_ {
public:
  DEFINE_CLASS_PRINTER(AST_Argument);
  AST_Argument_();
  virtual string print() const =0;
  void setFinal();
  void setEach();
  bool isFinal() const;
  bool hasEach() const; 
  /* Conversion with dynamics casts */
  GET_AS(Argument,Modification);
  GET_AS(Argument,Redeclaration);
private:
  bool _final,_each;
};

class AST_Argument_Modification_: public AST_Argument_ {
public:
  AST_Argument_Modification_(AST_String name, AST_Modification m);
  AST_String name() const;
  AST_Modification modification() const;
  string print() const;

private:
  AST_String _name;
  AST_Modification _mod;
};

class AST_Argument_Redeclaration_: public AST_Argument_ {
};

class AST_Modification_: public AST_Node_ {
public:
  DEFINE_CLASS_PRINTER(AST_Modification);
  virtual ModificationType modificationType();
  virtual string print() const =0;

  /* Conversion with dynamics casts */
  GET_AS(Modification,Equal);
  GET_AS(Modification,Class);
  GET_AS(Modification,Assign);
};

class AST_Modification_Equal_: public AST_Modification_ {
public:
  AST_Modification_Equal_(AST_Expression e);
  AST_Expression exp();
  string print() const;
  virtual ModificationType modificationType();

private:
  AST_Expression _e;
};

class AST_Modification_Assign_: public AST_Modification_ {
public:
  AST_Modification_Assign_(AST_Expression e);
  AST_Expression exp();
  string print() const;
  virtual ModificationType modificationType();

private:
  AST_Expression _e;
};

class AST_Modification_Class_: public AST_Modification_ {
public:
  AST_Modification_Class_(AST_ArgumentList al, AST_Expression e);
  AST_Expression exp() const;
  AST_ArgumentList arguments() const;
  string print() const;
  void setArguments(AST_ArgumentList al);
  virtual ModificationType modificationType();

private:
  AST_Expression _e;
  AST_ArgumentList _al;
};


#endif

