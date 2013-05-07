
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

Type_Tupla Type_::getAsTupla() {
  return dynamic_cast<Type_Tupla_*>(this);
}

Type_Function Type_::getAsFunction() {
  return dynamic_cast<Type_Function_ *>(this);
}


int operator==( Type_ &e1 ,  Type_ &e2 ){
    if (e1.getType() == e2.getType()) {
		switch (e1.getType())
		{
			case TYARRAY:
				return ( *(e1.getAsArray()->arrayOf()) == e2.getAsArray()->arrayOf() );
			case TYTUPLA:
			{
				Type_Tupla t1 = e1.getAsTupla(),t2 = e2.getAsTupla();
				if ( t2->tupla()->size() != t1->tupla()->size() ) return 0;
				TypeListIterator it1 = t1->tupla()->begin() , it2 = t1->tupla()->begin();
				foreach(it1,t1->tupla()) {
					if ( *current(it1)  != current(it2) ) return 0;
					it2++;
				}
				return 1;
			}
			case TYFUNCTION: // No es necesario!!
			{
				Type_Function f1 = e1.getAsFunction(),f2 = e2.getAsFunction();
				return (  *(f1->output()) == f2->output() );
			}
			default:
				return 1;	
		}
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


Type_Tupla_::Type_Tupla_(TypeList tyl): _tyl(tyl) {};

string Type_Tupla_ :: print() const{
	stringstream ret(stringstream::out);
	TypeListIterator tyit;
	int i = 0 , s = _tyl->size();
	ret << "< " ;
	foreach(tyit , _tyl) {
		i++;
		ret << current(tyit);
		if (i < s) ret << " , " ;
	}
	ret << " > " ;
    return ret.str();
}

Type_Function_::Type_Function_(Type o , TypeList i): _input(i) , _output(o)  {};

string Type_Function_ :: print() const{
	stringstream ret(stringstream::out);
	TypeListIterator tyit;
	int i = 0 , s = _input->size();
	
	ret << _output << "  function ";
	
	ret << "( " ;
	foreach(tyit , _input) {
		i++;
		ret << current(tyit);
		if (i < s) ret << " , " ;
	}
	ret << " ) " ;
    return ret.str();
}

/*
int operator==( Type &e1 ,  Type &e2 ){
    return (*e1 == * e2);
}
*/

