
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

#include <map>
#include <string>
#include <util/type.h>
#include <ast/ast_node.h>
#include <ast/ast_builder.h>

#ifndef SymbolTable_H
#define SymbolTable_H


DEFINE_TYPE(VarSymbolTable);
DEFINE_TYPE(TypeSymbolTable);
DEFINE_TYPE(VarInfo);


typedef std::string VarName;
typedef std::string TypeName;


template<class Key,class Value>
class SymbolTable {
public:
  void insert(Key k, Value v) {
    _st[k]=v;
  }
  Value lookup(Key k) {
    typename std::map<Key, Value>::iterator it;
    it = _st.find(k);
    if (it==_st.end()) return NULL;
    else               return it->second;
  }
  int count() {
    return _st.size();
  }
  Value val(int i) {
    typename std::map<Key, Value>::iterator it;
    it = _st.begin();
    for (int j=0; j<i; j++) it++;
    return it->second;
  }
  Key key(int i) {
    typename std::map<Key, Value>::iterator it;
    it = _st.begin();
    for (int j=0; j<i; j++) it++;
    return it->first;
  }
  void remove(Key k) {
    _st.erase(k);
  }

protected:
  std::map<Key,Value> _st;
};


class VarInfo_ {
public:
  VarInfo_( Type t, AST_TypePrefix tp, AST_Modification, AST_Comment);
  AST_TypePrefix typePrefix() {
    return _tp;
  };

  AST_Comment comment() {
    return _comm;
  }
  void setComment(AST_Comment c) {
    _comm=c;
  }

  AST_Modification modification() {
    return _m;
  };
  void setModification (AST_Modification m) {
    _m=m;
  };

  Type type();
  void setType (Type t) {
    _t=t;
  };

  void setParameter() {
    _tp = TP_PARAMETER;
  }
  bool isParameter() const {
    return _tp & TP_PARAMETER;
  }
  bool isDiscrete() const  {
    return (_tp & TP_DISCRETE) || _discrete;
  }
  bool builtIn() const {
    return _builtin;
  }
  /**
  * Para el caso de una variable real la cual es asignada en una clausula when.
  */
  void setDiscrete();
  void setBuiltIn() {
    _builtin=true;
  }
  bool isConstant() const {
    return _tp & TP_CONSTANT;
  }
  bool isInput() const {
    return _tp & TP_INPUT;
  }
  bool isOutput() const {
    return _tp & TP_OUTPUT;
  }

  bool isState();
  void setState();
  
  bool isUnknown();
  void setUnknown();

  friend ostream & operator<<(ostream &os , const VarInfo_ &e );
private:
  bool _state;
  bool _unknown;
  bool _discrete;
  Type _t;
  AST_TypePrefix _tp ;
  AST_Modification _m;
  AST_Comment _comm;
  bool _builtin;
};


class VarSymbolTable_: public SymbolTable<VarName, VarInfo  >
{
public:
  VarSymbolTable_() {};
  /* Crea las Variables del sistema */
  void initialize(TypeSymbolTable);

  VarInfo varInfo(int i) {
    return val(i);
  }
  VarName varName(int i) {
    return key(i);
  }
};

class TypeSymbolTable_: public SymbolTable<TypeName, Type >
{
public:
  TypeSymbolTable_();
};

VarInfo newVarInfo( Type t, AST_TypePrefix tp, AST_Modification, AST_Comment);
VarSymbolTable  newVarSymbolTable();
TypeSymbolTable newTypeSymbolTable();

#endif
