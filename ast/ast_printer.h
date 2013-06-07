
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

#ifndef AST_PRINTER
#define AST_PRINTER

#include <iostream>
#include <list>
#include <string>

using namespace std;

/* Printers */
#define DEFINE_PRINTER(X) ostream & operator<<(ostream &os , const X &x );
#define DEFINE_CLASS_PRINTER(X) friend ostream & operator<<(ostream &os , const X##_ &x );

#define CLASSP_PRINTER_IMP(X) ostream & operator<<(ostream &os , const X &e ){ \
                                os << *e; return os;  }
#define CLASS_PRINTER_IMP(X) ostream & operator<<(ostream &os, \
                                    const X##_ &e ){ os << e.print(); return os; } \
                             CLASSP_PRINTER_IMP(X) 

DEFINE_PRINTER(AST_Argument);
DEFINE_PRINTER(AST_Comment);
DEFINE_PRINTER(AST_CompositionElement);
DEFINE_PRINTER(AST_Composition);
DEFINE_PRINTER(AST_Class);
DEFINE_PRINTER(AST_Element);
DEFINE_PRINTER(AST_Expression);
DEFINE_PRINTER(AST_Expression_ComponentReference);
DEFINE_PRINTER(AST_Equation);
DEFINE_PRINTER(AST_Modification);
DEFINE_PRINTER(AST_Statement);
DEFINE_PRINTER(AST_StoredDefinition);
DEFINE_PRINTER(AST_String);

#endif
