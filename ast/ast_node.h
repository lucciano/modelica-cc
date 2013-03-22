#ifndef AST_NODE
#define AST_NODE
#include <iostream>
#include <parser/mcc_parser.h>
#include <ast/ast_builder.h>

using namespace std;

class AST_Node {
public:
  AST_Node() {
    _linenum = parser->lineno();
  }
  friend ostream & operator<<(ostream &os , const AST_Node &n ) { os << "Printing not implemented!!"<< endl; return os;};
  void setLineNum(int linenum) { 
    _linenum = linenum;
  } 
  int lineNum() const {
    return _linenum;
  }

private:    
  int _linenum;
};
#endif
