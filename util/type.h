
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

#include <string>
#include <list>
#include <ast/ast_node.h>
#include <ast/ast_builder.h>
#include <ast/expression.h>

#ifndef TYPE_H
#define TYPE_H


enum TypesType { TYREAL , TYINTEGER, TYBOOLEAN , TYSTRING , TYARRAY };


class Type_;
class Type_Real_;
class Type_Integer_;
class Type_Boolean_;
class Type_String_;
class Type_Array_;


typedef Type_                   * Type;
typedef Type_Real_              * Type_Real;
typedef Type_Integer_           * Type_Integer;
typedef Type_Boolean_           * Type_Boolean;
typedef Type_String_            * Type_String;
typedef Type_Array_             * Type_Array;


class Type_ {
public:
	virtual TypesType getType() = 0;
	virtual string print() const =0;
	friend ostream & operator<<(ostream &os , const Type_ &e );
	friend ostream & operator<<(ostream &os , const Type &e );
	
	friend int operator==( Type_ &e1 ,  Type_ &e2 );
	//friend int operator==( Type &e1 ,  Type &e2 );
	
	Type_Array getAsArray();
};

class Type_Real_ : public Type_ {
public:
	virtual TypesType getType() { return TYREAL; }
	string print() const;
};

class Type_Integer_ : public Type_ {
public:
	virtual TypesType getType() { return TYINTEGER; }
	string print() const;
};

class Type_Boolean_ : public Type_ {
public:
	virtual TypesType getType() { return TYBOOLEAN; }
	string print() const;
};

class Type_String_ : public Type_ {
public:
	virtual TypesType getType() { return TYSTRING ;}
	string print() const;
};

class Type_Array_ : public Type_ {
public:
	Type_Array_(Type t); 
	virtual TypesType getType() { return TYARRAY ;}
	string print() const;
	Type arrayOf();
private:
	Type _t;
};
#endif
