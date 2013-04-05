#ifndef AST_ELEMENT
#define AST_ELEMENT
#include <sstream>
#include <string>
#include <list>

#include <ast/ast_builder.h>
#include <ast/ast_node.h>
using namespace std;


ostream & operator<<(ostream &os , const AST_Element &e );
class AST_Class_;
class AST_Component_;
class AST_Element_: public AST_Node {
public:
  friend ostream & operator<<(ostream &os , const AST_Element_ &e ){
    os << e.print();  
    return os;
  };
  virtual string print() const =0;
  virtual ElementType elementType() { return ELNONE; }
  AST_Component getAsComponent();
  AST_ExtendsClause getAsExtendsClause();
  AST_Element_ClassWrapper getAsClass();
};

class AST_ExtendsClause_: public AST_Element_ {
public:
  AST_ExtendsClause_ (string name):_name(name) {};
  string print() const { 
    stringstream ret(stringstream::out);
    ret << "EXTENDS " << _name  ;
    return ret.str();
  }
  virtual ElementType elementType() { return EXTENDS; }
  string _name;
  AST_String name() { return &_name; }

};

class AST_ImportClause_: public AST_Element_ {
public:
  AST_ImportClause_ (string name):_name(name) {};
  string print() const { 
    stringstream ret(stringstream::out);
    ret << "IMPORT[" << _name << "]";
    return ret.str();
  }
  string _name;

};

class AST_Declaration_: public AST_Element_{
public:
  AST_Declaration_(string name):_name(name) {};
  string print() const { 
    stringstream ret(stringstream::out);
    ret << _name;
    return ret.str();
  }
  string _name;
  string name() { return _name; }
};

class AST_Component_: public AST_Element_ {
public:
  AST_Component_(AST_DeclarationList decl_list, string type, AST_TypePrefix tp):_decl_list(decl_list), 
                                                                                          _type(type), 
                                                                                          _inherited(false), 
                                                                                          _origin(NULL),
                                                                                          _tp(tp) {
 
  };
  string print() const { 
    stringstream ret(stringstream::out);
    AST_DeclarationListIterator it,itt;
    ret << "  "; 
    if (isParameter()) 
      ret << "parameter "; 
    ret << _type << " "; 
    for (it=_decl_list->begin();it!=_decl_list->end();it++) {
      itt=it;
      itt++;
      ret << current(it) << (itt==_decl_list->end() ? "" : ",");
    }
    ret << ";"; 
    return ret.str();
  }
  bool isFlat();
  bool isParameter() const { return _tp & TP_PARAMETER; }
  bool isInherited() const { return _inherited; }
  void setInherited() { _inherited=true; }
  string type() { return _type; };
  virtual ElementType elementType() { return COMPONENT; }
  string name() { return _decl_list->front()->name(); }
  AST_Class origin () { return _origin; }
  void setOrigin(AST_Class c) { if (_origin==NULL) _origin = c; }
  AST_TypePrefix typePrefix() { return _tp; }

  bool _inherited;
  AST_Class _origin;
  AST_TypePrefix _tp;
  AST_DeclarationList _decl_list;
  string _type;
  
};

class AST_Element_ClassWrapper_: public AST_Element_ {
public:
  AST_Element_ClassWrapper_(AST_Class c);
  string print() const ; 
  virtual ElementType elementType() { return ELCLASS; }
  AST_Class getClass() { return _c; }
private:
  AST_Class _c; 
};
#endif
