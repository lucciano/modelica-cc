
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
using namespace std;


ostream & operator<<(ostream &os , const AST_Element &e );
class AST_Class_;
class AST_Component_;

class AST_Element_: public AST_Node {
public:
  friend ostream & operator<<(ostream &os , const AST_Element_ &e );
  virtual string print() const =0;
  virtual ElementType elementType() { return ELNONE; }
  AST_Component getAsComponent();
  AST_ExtendsClause getAsExtendsClause();
  AST_Element_ClassWrapper getAsClass();
};

class AST_ExtendsClause_: public AST_Element_ {
public:
  AST_ExtendsClause_ (string name);
  string print() const;
  virtual ElementType elementType() { return EXTENDS; }
  AST_String name() { return &_name; }

private:
  string _name;
};

class AST_ImportClause_: public AST_Element_ {
public:
  AST_ImportClause_ (string name);
  string print() const;

private:
  string _name;
};

class AST_Declaration_: public AST_Element_{
public:
  AST_Declaration_(string name, AST_ExpressionList indexes, AST_Modification);
  string print() const;
  string name() const { return _name; }
  AST_ExpressionList indexes() const { return _indexes;}
  AST_Modification modification() const { return _mod; }
  void setComment(AST_Comment c) { _comm = c; }
  AST_Comment comment() const { return _comm; }
private:
  string _name;
  AST_ExpressionList _indexes;
  AST_Modification _mod;
  AST_Comment _comm;
};

class AST_Component_: public AST_Element_ {
public:
  AST_Component_(AST_DeclarationList decl_list, string type, AST_TypePrefix tp, AST_ExpressionList index);
  string print() const;
  bool isParameter() const { return _tp & TP_PARAMETER; }
  bool isInput() const { return _tp & TP_INPUT; }
  bool isOutput() const { return _tp & TP_OUTPUT; }
  bool isDiscrete() const { return _tp & TP_DISCRETE; }
  bool isFlow() const { return _tp & TP_FLOW; }
  bool isStream() const { return _tp & TP_STREAM; }
  bool isConstant() const { return _tp & TP_CONSTANT; }
  bool isInherited() const { return _inherited; }
  void setInherited() { _inherited=true; }
  string type() { return _type; };
  virtual ElementType elementType() { return COMPONENT; }
  string name() { return _decl_list->front()->name(); }
  AST_DeclarationList nameList() { return _decl_list; }
  AST_Class origin () { return _origin; }
  void setOrigin(AST_Class c) { if (_origin==NULL) _origin = c; }
  AST_TypePrefix typePrefix() { return _tp; }
  AST_ExpressionList indexes() const {return _indexes;}

private:
  bool _inherited;
  AST_Class _origin;
  AST_TypePrefix _tp;
  AST_DeclarationList _decl_list;
  AST_ExpressionList _indexes;
  string _type;
  
};

class AST_Element_ClassWrapper_: public AST_Element_ {
public:
  AST_Element_ClassWrapper_(AST_Class c);
  string print() const ; 
  virtual ElementType elementType() { return ELCLASS; }
  AST_Class getClass() { return _c; }

private:
  AST_Class _c; 
};

class AST_Comment_: public AST_Node {
public:
  AST_Comment_(AST_String st, AST_ArgumentList al): _st(st), _al(al) {}
  AST_ArgumentList arguments() const { return _al; }
  AST_String string()  const{ return _st; }
  friend ostream & operator<<(ostream &os , const AST_Comment &c );
private:
  AST_String _st;
  AST_ArgumentList _al;
};
#endif
