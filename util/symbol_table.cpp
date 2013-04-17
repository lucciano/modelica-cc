
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

#include <util/symbol_table.h>


VarInfo::VarInfo (bool p , bool s , Type t):_p(p) , _s(s) , _t(t) {};

bool VarInfo::isParameter() {return _p;};

bool VarInfo::isState() {return _s;};

Type VarInfo::type() {return _t;};


template <class Key, class Value>  
void SymbolTable::insert(Key k, Value v) {
  _st[k]=v;
};

template<class Key,class Value>
Value SymbolTable::lookup(Key k) {

  map<Key, Value>::iterator it;
  it = _st.find(k);
  if (it==_st.end()) return null;
  else               return it->second;
};