#include <sstream>
#include <list>

#include <ast/ast_node.h>
#include <ast/expression.h>
/*const char *BinOpTypeName[] =  {"BINOPLOWER", "BINOPLOWEREQ", "BINOPGREATER", "BINOPGREATEREQ", "BINOPCOMPNE", "BINOPCOMPEQ", "BINOPDIV", "BINOPELDIV", "BINOPMULT",
                          "BINOPELMULT", "BINOPADD", "BINOPELADD", "BINOPSUB", "BINOPELSUB","BINOPEXP", "BINOPELEXP", "BINOPAND", "BINOPOR" };
*/

const char *BinOpTypeName[] =  {"<", "<=", ">", ">=", "<>", "==", "/", "./", "*",
                          ".*", "+", ".+", "-", ".-","^", ".^", " and ", " or " };

using namespace std;
ostream & operator<<(ostream &os , const AST_Expression_ &e ){
    os << e.print();  
    return os;
}

ostream & operator<<(ostream &os , const AST_Expression &e ){
    os << *e;
    return os;
}
AST_Expression_BinOp_::AST_Expression_BinOp_(AST_Expression e1,AST_Expression e2, BinOpType t): _e1(e1), _e2(e2), _t(t) {
}


string AST_Expression_BinOp_::print() const { 
    stringstream ret(stringstream::out);
    ret << left() << BinOpTypeName[binopType()] << right();
    return ret.str();
}

AST_Expression_Integer_::AST_Expression_Integer_(int i):_i(i) {
};

string AST_Expression_Integer_::print() const { 
  stringstream ret(stringstream::out);
  //ret << "INTEGER[" << _i << "]";
  ret << _i ;
  return ret.str();
}

AST_Expression_Real_::AST_Expression_Real_(double d):_d(d) {
}

string AST_Expression_Real_::print() const { 
  stringstream ret(stringstream::out);
  ret << "REAL[" << _d << "]";
  return ret.str();
}

AST_Expression_String_::AST_Expression_String_(string s): _s(s){
}

string AST_Expression_String_::print() const { 
  stringstream ret(stringstream::out);
  ret << "STRING[" << _s << "]";
  return ret.str();
}

AST_Expression_Boolean_::AST_Expression_Boolean_(bool b):_b(b){
}

string AST_Expression_Boolean_::print() const { 
  stringstream ret(stringstream::out);
  ret << "BOOLEAN[" << (_b ? "true" : "false") << "]";
  return ret.str();
}

AST_Expression_Derivative_::AST_Expression_Derivative_(AST_ExpressionList el):_el(el){
}

string AST_Expression_Derivative_::print() const { 
  stringstream ret(stringstream::out);
  ret << "der(" << *_el->front() << ")";
  return ret.str();
};

AST_Expression_UMinus_::AST_Expression_UMinus_(AST_Expression e): _e(e){
};

string AST_Expression_UMinus_::print() const { 
  stringstream ret(stringstream::out);
  ret << "-(" <<  _e << ")";
  return ret.str();
};

AST_Expression_BooleanNot_::AST_Expression_BooleanNot_(AST_Expression e):_e(e){};
string AST_Expression_BooleanNot_::print() const { 
  stringstream ret(stringstream::out);
  ret << "not " <<  exp();
  return ret.str();
}

AST_Expression_Call_::AST_Expression_Call_(AST_String name):_name(name){ };
string AST_Expression_Call_::print() const {
  stringstream ret;
  ret << "CALL[" << name() << "]";
  return ret.str();
}

AST_Expression_ComponentReference_::AST_Expression_ComponentReference_(string name):_name(name){};
AST_Expression_ComponentReference_::AST_Expression_ComponentReference_():_name(""){};
string AST_Expression_ComponentReference_::print() const {
  stringstream ret;
  //ret << "COMPONENTREF[" << _name << "]";
  ret <<  _name ;
  return ret.str();
}
#include <stdlib.h>
void AST_Expression_ComponentReference_::append(AST_Expression_ComponentReference_ *cr) { 
  _name.append(cr->_name);
}

void AST_Expression_ComponentReference_::append(string s) {
  _name.append(s);
}


AST_Expression_ComponentReference AST_Expression_::getAsComponentRef() {
  return dynamic_cast<AST_Expression_ComponentReference>(this);
}


void AST_Expression_ComponentReference_::setName(string name) { _name=name; }

ExpressionType AST_Expression_::expressionType() { return EXPNONE; }

AST_Expression AST_Expression_::duplicateExpression() {
  return NULL;
}

AST_Expression AST_Expression_ComponentReference_::duplicateExpression() { 
  return newAST_Expression_ComponentReference(newAST_String(name()));
}
AST_Expression AST_Expression_Integer_::duplicateExpression() {
  return newAST_Expression_Integer(val());
}
AST_Integer AST_Expression_Integer_::val() {
  return _i;
}

AST_Expression_Derivative AST_Expression_::getAsDerivative() {
  return dynamic_cast<AST_Expression_Derivative>(this);
};

AST_Expression AST_Expression_Derivative_::duplicateExpression() {
  AST_ExpressionList new_el = newAST_ExpressionList();
  AST_ExpressionList el     = arguments();
  AST_ExpressionListIterator it;
  foreach (it,el) 
    AST_ListAppend(new_el,current(it)->duplicateExpression());
    
  return newAST_Expression_Derivative(new_el);

}

/* If expression */
AST_Expression_If_::AST_Expression_If_(AST_Expression cond, AST_Expression then, AST_Expression else_exp): _cond(cond), 
                                                                                                           _then(then), 
                                                                                                           _else_exp(else_exp) {
}

string AST_Expression_If_::print () const {
  stringstream ret;
  ret << "if ("<< condition() << ") then " << then() << " else " << else_exp();
  return ret.str();
}

ExpressionType AST_Expression_If_::expressionType() {
  return EXPIF;
}

ExpressionType AST_Expression_Call_::expressionType() {
  return EXPCALL;
}

string AST_Expression_End_::print() const { 
  return "end";
}
