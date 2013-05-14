
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

#include <ast/modification.h>

AST_ModificationEqual_:: AST_ModificationEqual_(AST_Expression e): _e(e) {
}

AST_ModificationAssign_:: AST_ModificationAssign_(AST_Expression e): _e(e) {
}

AST_ModificationClass_:: AST_ModificationClass_(AST_ArgumentList al,AST_Expression e): _e(e), _al(al) {
}

GET_AS_IMP(Modification,Equal);
GET_AS_IMP(Modification,Class);
GET_AS_IMP(Modification,Assign);
