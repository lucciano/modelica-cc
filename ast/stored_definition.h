#ifndef STORED_DEFINITION
#define STORED_DEFINITION
#include <ast/ast_node.h>
#include <ast/class.h>

class AST_StoredDefinition_: public AST_Node
{
public:
  AST_StoredDefinition_(AST_ClassList ml, AST_String within);
  friend ostream & operator<<(ostream &, const AST_StoredDefinition_ &) ;
  AST_ClassList models() const { return _model_list; }
  AST_String within() const { return _within; }
private:
  AST_ClassList _model_list;
  AST_String _within;
} ;
#endif
