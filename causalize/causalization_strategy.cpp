/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <boost/lambda/lambda.hpp>

#include <causalize/causalization_strategy.h>
#include <ast/equation.h>
#include <causalize/compref_occurrence_traverse.h>
#include <causalize/debug.h>

#include <iostream>

CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations,
		AST_ExpressionList unknowns) {

	if (equations->size() != unknowns->size()) {
	  ERROR ("The model being causalizing is not balanced."); // TODO No deberiamos lanzar una excepción acá?
	}

  int index = 0;

  _acausalEqs = new list<Vertex>;
  _unknowns = new list<Vertex>;

  _causalEqs1 = newMMO_EquationList();
  _causalEqsN = newMMO_EquationList();

  MMO_EquationListIterator eqIter;
  AST_ExpressionListIterator expIter;

  DEBUG('c', "Building causalization graph...\n");

  DEBUG('c', "Equation indexes:\n");

  foreach(eqIter, equations) {
    VertexProperties *vp = new VertexProperties;
    vp->eq = current_element(eqIter);
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _acausalEqs->push_back(v);
    DEBUG('c', "%d: %s", vp->index, vp->eq->print().c_str());
  }

  DEBUG('c', "Unknown indexes:\n");

  index = 0;
  foreach(expIter, unknowns) {
    VertexProperties *vp = new VertexProperties;
    vp->unknown = current_element(expIter);
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _unknowns->push_back(v);
    DEBUG('c', "%d: %s\n", vp->index, vp->unknown->print().c_str());
   }

  DEBUG('c', "Graph edges:\n");

  list<Vertex>::iterator acausalEqsIter, unknownsIter;
  foreach(acausalEqsIter, _acausalEqs) {
    foreach(unknownsIter, _unknowns) {
      Vertex eqVertex = current_element(acausalEqsIter);
      Vertex unknownVertex = current_element(unknownsIter);
      if(occur(_graph[unknownVertex].unknown, _graph[eqVertex].eq)) {
        add_edge(eqVertex, unknownVertex, _graph);
        DEBUG('c', "(%d, %d) ", _graph[eqVertex].index, _graph[unknownVertex].index);
      }
    }
  }
  DEBUG('c', "\n");

}

CausalizationStrategy::~CausalizationStrategy() {
}

MMO_EquationList CausalizationStrategy::causalize() {

  if(_acausalEqs->empty()) {
    _causalEqs1->insert(_causalEqs1->end(), _causalEqsN->begin(), _causalEqsN->end());
    return _causalEqs1;
  }

  int acausalEqsSize = _acausalEqs->size();

  list<Vertex>::iterator iter, auxiliaryIter;

  auxiliaryIter = _acausalEqs->begin();
  for(iter = auxiliaryIter; iter != _acausalEqs->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex eq = current_element(iter);
    if (out_degree(eq, _graph) == 1) {
      Edge e = *out_edges(eq, _graph).first;
      Vertex unknown = target(e, _graph);
      remove_out_edge_if(unknown, boost::lambda::_1 != e, _graph);
      _causalEqs1->push_back(_graph[eq].eq);
      _acausalEqs->erase(iter);
      _unknowns->remove(unknown);
    }
  }

  auxiliaryIter = _unknowns->begin();
  for(iter = auxiliaryIter; iter != _unknowns->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex unknown = current_element(iter);
    if(out_degree(unknown, _graph) == 1) {
      Edge e = *out_edges(unknown, _graph).first;
      Vertex eq = target(e, _graph);
      remove_out_edge_if(eq, boost::lambda::_1 != e, _graph);
      _causalEqsN->push_front(_graph[eq].eq);
      _acausalEqs->remove(eq);
      _unknowns->erase(iter);
    }
  }

  if (acausalEqsSize == _acausalEqs->size()) {
    DEBUG('c', "Algebraic loop(s) detected. Processing...\n");

  }

  causalize();

}

bool CausalizationStrategy::occur(AST_Expression unknown, MMO_Equation equation) {
  CompRefOccurrenceTraverse *occurrenceTraverse = new CompRefOccurrenceTraverse(unknown);
  switch (equation->equationType()) {
    case EQEQUALITY:
    {
      AST_Equation_Equality eqEquality = equation->getAsEquality();
      return occurrenceTraverse->foldTraverse(eqEquality->left()) || occurrenceTraverse->foldTraverse(eqEquality->right());
    }
    default:
      ;// TODO [MOYA]
  }
  return false;
}

