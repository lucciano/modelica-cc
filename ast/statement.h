#ifndef AST_STATMENT
#define AST_STATMENT
#include <ast/ast_node.h>
#include <ast/ast_builder.h>

class AST_Statement_: public AST_Node
{
public:
  friend ostream & operator<<(ostream &os , const AST_Statement_ &s ){
    os << s.print();  
    return os;
  };
  virtual string print() const =0;
  //virtual EquationType equationType() = 0;
 // virtual AST_Equation duplicateEquation();
  //AST_Equation_Equality getAsEquality();
  //AST_Equation_Connect getAsConnect();
};


class AST_Statement_Return_: public AST_Statement_ {
  string print() const { return "return"; }
};

class AST_Statement_Break_: public AST_Statement_ {
  string print() const { return "return"; }
};
#endif
