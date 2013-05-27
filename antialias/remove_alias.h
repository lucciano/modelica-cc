#include <iostream>
#include <mmo/mmo_class.h>
#include <util/symbol_table.h>

using namespace std;

class RemoveAlias {
public:
	void removeAliasEquations(MMO_Class c);;
private:
  void addAlias(AST_Expression, AST_Expression);
	MMO_Class _c;
	VarSymbolTable _varSymbolTable;
};

