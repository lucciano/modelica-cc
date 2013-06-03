
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


VarInfo_::VarInfo_ ( Type t, AST_TypePrefix tp, AST_Modification m, AST_Comment c): _state(false), _discrete(false) , _t(t) , _tp(tp) , _m(m),_comm(c), _builtin(false) {
};




bool VarInfo_::isState() {return _state;};

void VarInfo_::setState() { _state = true; };

void VarInfo_::setDiscrete() {
	_discrete = true;
}

Type VarInfo_::type() {return _t;};

TypeSymbolTable_::TypeSymbolTable_(){
	insert("String",  new Type_String_()  );
	insert("Real",    new Type_Real_()    );
	insert("Integer", new Type_Integer_() );
	insert("Boolean", new Type_Boolean_() );
}

void VarSymbolTable_::initialize(TypeSymbolTable ty) {
  VarInfo v=newVarInfo(ty->lookup("Real"),0,NULL,NULL);
  v->setBuiltIn();
  insert("time",v);
}
;
ostream & operator<<(ostream &ret , const VarInfo_ &e )
{
	if (e.isParameter()) ret << "parameter ";
	if (e.isDiscrete())  ret << "discrete ";
	if (e.isConstant())  ret << "constant ";
	if (e.isInput())     ret << "input ";
	if (e.isOutput())    ret << "output ";
	ret << e._t;
	return ret;
}


VarInfo newVarInfo( Type t, AST_TypePrefix tp, AST_Modification m , AST_Comment c){
	return new VarInfo_(t,tp,m,c);
}

VarSymbolTable newVarSymbolTable()
{
	return new VarSymbolTable_;
}
TypeSymbolTable newTypeSymbolTable()
{
	return new TypeSymbolTable_;
}
