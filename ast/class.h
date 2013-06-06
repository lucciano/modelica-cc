
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
#include <ast/composition.h>
#include <ast/equation.h>

#ifndef AST_CLASS
#define AST_CLASS

class AST_Class_: public AST_Node_ {
public: 
  AST_Class_(AST_String n,AST_Composition comp);

  void setComposition(AST_Composition c);
  AST_Composition composition() const;

  AST_String name() const;

  AST_Element_ComponentList getComponents();
  void addComponent(AST_Element_Component c);

  /*
  void addEquation(AST_Equation e);
  void removeEquation(AST_Equation e);
  */

  void addClass(AST_Class c);
  AST_ClassList getClasses();

  void setFather(AST_Class c);
  AST_Class father() const;
  
  void addExtends(AST_String e);
  AST_StringList getExtends();

  bool isBasic();
  void setBasic();

  friend ostream & operator<<(ostream &os , const AST_Class_ &cl );
  
  void setFinal() { _final=true; }
  void setEncapsulated() { _encapsulated=true; }
  void setPrefixes(AST_ClassPrefix cp) { _prefix=cp; }
  AST_ClassPrefix prefix() const { return _prefix; }

private:
  AST_Element_ComponentList _components;
  AST_StringList    _extends_list;
  AST_String        _name;
  AST_Composition   _composition;
  AST_ClassList     _sub_classes;
  AST_Class         _father;
  AST_Boolean _basic,_final,_encapsulated;
  AST_ClassPrefix _prefix;
};
#endif
