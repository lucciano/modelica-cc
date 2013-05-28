
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


VarInfo::VarInfo ( Type t, AST_TypePrefix tp, AST_Modification m, AST_Comment c): _state(false), _discrete(false) , _t(t) , _tp(tp) , _m(m),_comm(c) {
};

bool VarInfo::isState() {return _state;};

void VarInfo::setState() { _state = true; };

void VarInfo::setDiscrete() {
	_discrete = true;
}

Type VarInfo::type() {return _t;};

TypeSymbolTable_::TypeSymbolTable_(){
	insert("String",  new Type_String_()  );
	insert("Real",    new Type_Real_()    );
	insert("Integer", new Type_Integer_() );
	insert("Boolean", new Type_Boolean_() );
}

void VarSymbolTable_::initialize(TypeSymbolTable ty) {
  insert("time",new VarInfo(ty->lookup("Real"),0,NULL,NULL));
}

ostream & operator<<(ostream &ret , const VarInfo &e )
{
	if (e.isParameter()) ret << "parameter ";
	if (e.isDiscrete())  ret << "discrete ";
	if (e.isConstant())  ret << "constant ";
	if (e.isInput())     ret << "input ";
	if (e.isOutput())    ret << "output ";
	ret << e._t;
	//if (e._m != NULL) os << e._m->print();
	return ret;
}
