#include <ast/equation.h>
#include <ast/ast_builder.h>


AST_Equation_Equality AST_Equation_::getAsEquality() {
  return dynamic_cast<AST_Equation_Equality_*>(this);
}

AST_Equation_Connect AST_Equation_::getAsConnect() {
  return dynamic_cast<AST_Equation_Connect_*>(this);
}

AST_Expression AST_Equation_Equality_::left() const {
  return _left;
}
AST_Expression AST_Equation_Equality_::right() const {
  return _right;
}


AST_Equation AST_Equation_::duplicateEquation() { 
  switch (this->equationType()) {
    case EQEQUALITY:
      AST_Equation_Equality eq = this->getAsEquality(); 
      return newAST_Equation_Equality(eq->left()->duplicateExpression(),eq->right()->duplicateExpression());
  }
}

string AST_Equation_If_::print() const {
  stringstream ret(stringstream::out);
  AST_EquationList eql = equationList();
  AST_EquationListIterator it;
  ret << "if (" << condition() << ") " << endl;
  foreach(it,eql)
    ret << "\t" << current(it) << endl;
  return ret.str();
}

ostream & operator<<(ostream &os , const AST_Equation &e ) {
  os << *e;
  return os;
}

AST_Equation_Call_::AST_Equation_Call_(AST_Expression e):_call(e) {

};

AST_Expression AST_Equation_Call_::call () const { 
  return _call;
}

string AST_Equation_Call_::print () const {
  stringstream ret(stringstream::out);
  ret << "EQCALL(" << call() << ") " << endl;
  return ret.str();
}


string AST_Equation_For_::print() const {
  stringstream ret(stringstream::out);
  AST_EquationList eql = equationList();
  AST_EquationListIterator it;
  ret << "for loop" << endl;
  foreach(it,eql)
    ret << "\t" << current(it) << endl;
  ret << "end for;"<< endl;
  return ret.str();

  return ret.str();
}
