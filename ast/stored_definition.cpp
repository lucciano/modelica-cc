
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

#include <ast/ast_builder.h>
#include <ast/stored_definition.h>

AST_StoredDefinition_:: AST_StoredDefinition_(AST_ClassList ml, AST_String within): _model_list(ml), _within(within) {
};

ostream & operator<<(ostream &os , const AST_StoredDefinition &sd )  {
  os << *sd;
  return os;

}
ostream & operator<<(ostream &os , const AST_StoredDefinition_ &sd )  {
  AST_ClassListIterator it;
  AST_ClassList         cl = sd.models();
  if (sd.within()!=NULL)
    os << "within " << sd.within() << endl; 
  foreach(it,cl)
    os << current(it);
  return os;
}

