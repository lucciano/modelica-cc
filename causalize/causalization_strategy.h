/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/graph_definition.h>

using namespace std;

class CausalizationStrategy {
public:
  CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns);
  ~CausalizationStrategy();
  void causalize(AST_String name, MMO_EquationList causalEqs, AST_ClassList cl);
private:
  void makeCausal1(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl);
  void makeCausalN(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl);
  void processCycles();
  CausalizationGraph _graph;
  list<Vertex> *_eqVertices;
  list<Vertex> *_unknownVertices;
  MMO_EquationList _causalEqs1; // equations numbered starting from 1
  MMO_EquationList _causalEqsN; // equations numbered starting from n
  AST_ClassList _cl;
  AST_ExpressionList _all_unknowns;
};


