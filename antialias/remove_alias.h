#include <iostream>
#include <mmo/mmo_class.h>
#include <util/symbol_table.h>

using namespace std;

class RemoveAlias {
public:
	void removeAliasEquations(MMO_Class *c);;
private:
	MMO_Class *_c;
	VarSymbolTable _varSymbolTable;
};

