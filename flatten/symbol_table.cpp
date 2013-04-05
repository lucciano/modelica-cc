
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

#include <flatten/symbol_table.h>
#include <stdlib.h>

void SymbolTable::addSymbol(string name, AST_Class cl) {
  _st[name]=cl;

}

AST_Class SymbolTable::lookup(string name) {

  map<string, AST_Class>::iterator it;
  it = _st.find(name);
  if (it==_st.end()) {
   cerr << "Failed lookup of type: " << name << endl;
   exit(-1);
  }
  return it->second;
}

void SymbolTable::dump() {
  map<string, AST_Class>::iterator it;
  cerr << "Valid types = {";
  for(it=_st.begin();it!=_st.end();it++) {
    cerr << it->first << " ";
  }
  cerr << "}" << endl;

}
