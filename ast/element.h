
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

#ifndef AST_ELEMENT
#define AST_ELEMENT
#include <sstream>
#include <string>
#include <list>

#include <ast/ast_builder.h>
#include <ast/ast_node.h>

/* Base class for elements */
class AST_Element_: public AST_Node_ {
public:
  DEFINE_CLASS_PRINTER(AST_Element);
  virtual string print() const =0;
  virtual ElementType elementType();

  /* Dynamic casts */
  GET_AS(Element,Component);
  GET_AS(Element,ExtendsClause);
  GET_AS(Element,ImportClause);
  GET_AS(Element,ClassWrapper);
};

/* Extends clause */
class AST_Element_ExtendsClause_: public AST_Element_ {
public:
  AST_Element_ExtendsClause_ (string name);
  string print() const;
  virtual ElementType elementType();
  AST_String name();

private:
  string _name;
};

/* Import clause */
class AST_Element_ImportClause_: public AST_Element_ {
public:
  AST_Element_ImportClause_ (string name);
  string print() const;

private:
  string _name;
};

/* Declaration */
class AST_Declaration_: public AST_Element_ {
public:
  AST_Declaration_(string name, AST_ExpressionList indexes, AST_Modification);
  string print() const;
  string name() const;
  AST_ExpressionList indexes() const;
  AST_Modification modification() const;
  void setComment(AST_Comment c);
  AST_Comment comment() const;

private:
  string _name;
  AST_ExpressionList _indexes;
  AST_Modification _mod;
  AST_Comment _comm;
};

/* Component */
class AST_Element_Component_: public AST_Element_ {
public:
  AST_Element_Component_(AST_DeclarationList decl_list, string type, AST_TypePrefix tp, AST_ExpressionList index);
  string print() const;
  bool isParameter() const;
  bool isInput() const;
  bool isOutput() const;
  bool isDiscrete() const;
  bool isFlow() const;
  bool isStream() const;
  bool isConstant() const;
  string type();
  virtual ElementType elementType();
  string name();
  AST_DeclarationList nameList();
  AST_Class origin ();
  void setOrigin(AST_Class c);
  AST_TypePrefix typePrefix();
  AST_ExpressionList indexes() const;

private:
  AST_DeclarationList _decl_list;
  string _type;
  AST_ExpressionList _indexes;
  AST_Class _origin;
  AST_TypePrefix _tp;
};

/* Class element */
class AST_Element_ClassWrapper_: public AST_Element_ {
public:
  AST_Element_ClassWrapper_(AST_Class c);
  string print() const ;
  virtual ElementType elementType();
  AST_Class getClass();

private:
  AST_Class _c;
};

/* Comment */
class AST_Comment_: public AST_Node_ {
public:
  AST_Comment_(AST_String st, AST_ArgumentList al);
  AST_ArgumentList arguments() const;
  AST_String string()  const;
  void setAnnotation(AST_ArgumentList al);
  DEFINE_CLASS_PRINTER(AST_Comment);

private:
  AST_String _st;
  AST_ArgumentList _al;
};
#endif
