
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


typedef std::string VarName;
typedef int Type;

template<class Key,class Value>
class SymbolTable {
public:
  void insert(Key k, Value v);
  Value lookup(Key k);
private:
  std::map<Key,Value> _st;
};

class VarInfo {
  bool isParameter();
  bool isState();
  Type type();
};

class VarSymbolTable: public SymbolTable<VarName, VarInfo> 
{

};
