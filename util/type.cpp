
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

#include <util/type.h>

ostream & operator<<(ostream &os , const Type_ &e ){
    os << e.print();  
    return os;
}

ostream & operator<<(ostream &os , const Type &e ) {
  os << *e;
  return os;
}

string Type_Real_ :: print() const {
	stringstream ret(stringstream::out);
    ret << "Real" ;
    return ret.str();
}

string Type_Integer_ :: print() const{
	stringstream ret(stringstream::out);
    ret << "Integer" ;
    return ret.str();
}

string Type_Boolean_ :: print() const{
	stringstream ret(stringstream::out);
    ret << "Boolean" ;
    return ret.str();
}

string Type_String_ :: print() const{
	stringstream ret(stringstream::out);
    ret << "String" ;
    return ret.str();
}

string Type_Array_ :: print() const{
	stringstream ret(stringstream::out);
    ret << "Array [" <<  _dim <<  "]  of " << _t->print() ;
    return ret.str();
}

Type_Array_::Type_Array_(Type t, AST_Expression dim): _t(t) ,_dim(dim) {}; 

Type Type_Array_ ::arrayOf() { return _t;}

Type_Array Type_::getAsArray() {
  return dynamic_cast<Type_Array_*>(this);
}


int operator==( Type_ &e1 ,  Type_ &e2 ){
    if (e1.getType() == e2.getType()) {
		if (e1.getType() == TYARRAY)
			return (e1.getAsArray()->arrayOf() == e2.getAsArray()->arrayOf() );
		else return 1;	
	} else return 0; 
}

int operator==( Type_ &e1  ,  Type e2 ){
    return ( e1 == *e2);
}

int operator!=( Type_ &e1 ,  Type_ &e2 ){
    return !(e1 == e2);
}

int operator!=( Type_ &e1 ,  Type e2 ){
    return !(e1 == *e2);
}

/*
int operator==( Type &e1 ,  Type &e2 ){
    return (*e1 == * e2);
}
*/

