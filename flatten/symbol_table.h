#ifndef SYMBOL_TABLE
#define SYMBOL_TABLE

#include <map>
#include <ast/ast_builder.h>

using namespace std;

class SymbolTable {
public:
  void addSymbol (string name, AST_Class c);
  AST_Class lookup(string name);
  void dump();

private:
  map<string, AST_Class> _st;
};

#endif
