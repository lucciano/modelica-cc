#include <ast/ast_node.h>
#include <ast/element.h>
#include <ast/equation.h>
#ifndef AST_COMPOSITION
#define AST_COMPOSITION
#include <stdlib.h>

class AST_CompositionElement_: public AST_Node
{
public:
  AST_CompositionElement_(AST_CompositionEqsAlgs eqs_algs): _eqs_algs(eqs_algs) { 
    _el = newAST_ElementList();
  };
  AST_CompositionElement_(AST_ElementList el): _el(el) { 
    //_eqs_algs = newAST_EquationList();
  };
  
  friend ostream & operator<<(ostream &os , const AST_CompositionElement_ &ce ) ;
  AST_CompositionEqsAlgs getEquations();
  AST_CompositionEqsAlgs _eqs_algs;
  AST_ElementList _el;
};

class AST_Composition_: public AST_Node {
public:
  AST_Composition_ (AST_ElementList el,AST_CompositionElementList cl): _composition_list(cl), _element_list(el) {};
  friend ostream & operator<<(ostream &os , const AST_Composition_ &cm ) ;
  AST_CompositionElementList _composition_list;
  AST_CompositionElementList compositionList() const;
  AST_ElementList _element_list;
  AST_ElementList elementList() const;
  AST_EquationList getEquations();
  AST_ComponentList getComponents();
 
};

class AST_CompositionEqsAlgs_: public AST_Node {
public:
  AST_CompositionEqsAlgs_(AST_EquationList eq);
  friend ostream & operator<<(ostream &os , const AST_CompositionEqsAlgs &ceqa ) ;
  AST_EquationList getEquations() { return _eq; }
private:
  AST_EquationList _eq;
};
#endif
