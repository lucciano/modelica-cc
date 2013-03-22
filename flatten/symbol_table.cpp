#include <flatten/symbol_table.h>
#include <stdlib.h>

void SymbolTable::addSymbol(string name, AST_Class cl) {
  _st[name]=cl;

}

AST_Class SymbolTable::lookup(string name) {

  map<string, AST_Class>::iterator it;
  it = _st.find(name);
  if (it==_st.end()) {
   cerr << "Failed lookup of type: " << name << endl;
   exit(-1);
  }
  return it->second;
}

void SymbolTable::dump() {
  map<string, AST_Class>::iterator it;
  cerr << "Valid types = {";
  for(it=_st.begin();it!=_st.end();it++) {
    cerr << it->first << " ";
  }
  cerr << "}" << endl;

}
