#include <iostream>
#include <mmo/mmo_class.h>
#include <util/ast_util.h>
#include <util/symbol_table.h>

using namespace std;

class RemoveAlias {
public:
	RemoveAlias(MMO_Class *c);
	void removeAliasEquations();
private:
	MMO_Class *_c;
	VarSymbolTable _varSymbolTable;
};

