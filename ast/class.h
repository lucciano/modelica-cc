#include <ast/ast_builder.h>
#include <ast/composition.h>
#include <ast/equation.h>

#ifndef AST_CLASS
#define AST_CLASS

class AST_Class_: public AST_Node {
public: 
  AST_Class_(AST_String n,AST_Composition comp);

  void setComposition(AST_Composition c);
  AST_Composition composition() const;

  AST_String name() const;

  AST_ComponentList getComponents();
  void addComponent(AST_Component c);

  AST_EquationList  getEquations();
  void addEquation(AST_Equation e);
  void removeEquation(AST_Equation e);

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
private:
  AST_EquationList  _equations; 
  AST_ComponentList _components;
  AST_StringList    _extends_list;
  AST_Composition   _composition;
  AST_ClassList     _sub_classes;
  AST_Class         _father;
  AST_String        _name;
  AST_Boolean _basic,_final,_encapsulated;
  AST_ClassPrefix _prefix;
};
#endif
