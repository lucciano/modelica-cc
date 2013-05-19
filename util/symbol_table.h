
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

#ifndef SymbolTable_H
#define SymbolTable_H

class VarSymbolTable_;
class TypeSymbolTable_;


typedef std::string VarName;
typedef std::string TypeName;
typedef VarSymbolTable_    * VarSymbolTable;
typedef TypeSymbolTable_   * TypeSymbolTable;
//typedef int Type;

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
  int count() { return _st.size(); }
  Value val(int i) {
    typename std::map<Key, Value>::iterator it;
    it = _st.begin();
    for (int j=0;j<i;j++) it++;
    return it->second;
  }
  Key key(int i) {
    typename std::map<Key, Value>::iterator it;
    it = _st.begin();
    for (int j=0;j<i;j++) it++;
    return it->first;
  }

protected:
  std::map<Key,Value> _st;
};


class VarInfo {
public:  
  VarInfo( Type t, AST_TypePrefix tp);
  AST_TypePrefix typePrefix() {return _tp;};
  bool isParameter() { return _tp & TP_PARAMETER; }
  bool isDiscrete() { return (_tp & TP_DISCRETE) || _discrete; }
  /**
  * Para el caso de una variable real la cual es asignada en una clausula when.
  */
  void setDiscrete();
  bool isConstant() { return _tp & TP_CONSTANT; }
  bool isInput() { return _tp & TP_INPUT; }
  bool isOutput() { return _tp & TP_OUTPUT; }
  Type type();
  bool isState();
  void setState();
private:
  bool _state;
  bool _discrete;
  Type _t;
  AST_TypePrefix _tp ;
};


class VarSymbolTable_: public SymbolTable<VarName, VarInfo * > 
{
public:
	VarSymbolTable_() {};
  void initialize(TypeSymbolTable); 
  VarInfo *varInfo(int i) { return val(i); }
  VarName varName(int i) { return key(i); }
};

class TypeSymbolTable_: public SymbolTable<TypeName, Type > 
{
public:
	TypeSymbolTable_();
};


#endif
