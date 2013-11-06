/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/graph_definition.h>

using namespace std;

// TODO [Moya] Unit tests

class CausalizationStrategy {
public:
  CausalizationStrategy(MMO_Class mmo_class);
  ~CausalizationStrategy();
  void causalize(AST_String name, AST_ClassList cl);
private:
  bool findReferenceInEquation(AST_Expression unknown, MMO_Equation equation);
  void makeCausal1(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl);
  void makeCausalN(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl);
  void processCycles();
  void replaceMMOClassEquations();
  CausalizationGraph _graph;
  list<Vertex> *_eqVertices;
  list<Vertex> *_unknownVertices;
  MMO_Class _mmo_class;
  MMO_EquationList _causalEqs1; // equations numbered starting from 1
  MMO_EquationList _causalEqsN; // equations numbered starting from n
  AST_ClassList _cl;
  AST_ExpressionList _all_unknowns;
};


