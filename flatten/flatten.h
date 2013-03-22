#ifndef FLATTEN
#define FLATTEN

#include <ast/ast_builder.h>
#include <flatten/symbol_table.h>
#include <ast/equation.h>
class Flatten {
public:   
  Flatten();
  int flatten(AST_StoredDefinition sd);
  
private:
  void preFlat(AST_Class c);
  void doFlat(AST_Class c);
  void replaceComponentRefs(string name, AST_Equation, AST_Class c);
  void replaceComponentRefs(string name, AST_Expression, AST_Class c);
  AST_Component replaceComponent(string name, AST_Component);
  void expandConnections(AST_EquationConnect eq, AST_Class c) ;
  AST_StringList tsort(AST_Class);
  bool isLocalTo(string,AST_Class);
  AST_Equation duplicateEquation(AST_Equation e) { return e->duplicateEquation(); }
  SymbolTable components;
  SymbolTable types;
};

#endif
