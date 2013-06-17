/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/causalization_strategy.h>
#include <ast/equation.h>
#include <causalize/compref_occurrence_traverse.h>
#include <causalize/debug.h>

#include <iostream>

CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations,
		AST_ExpressionList unknowns) {

	// TODO [Moya] check #equations = # unknowns

  int index = 0;

  _acausalEqs = new list<Vertex>;
  _unknowns = new list<Vertex>;

  _causalEqs1 = newMMO_EquationList();
  _causalEqsN = newMMO_EquationList();

  MMO_EquationListIterator eqIter;
  AST_ExpressionListIterator expIter;

  DEBUG('c', INFO, "Building causalization graph...\n");

  DEBUG('c', INFO, "Equation indexes:\n");

  foreach(eqIter, equations) {
    VertexProperties *vp = new VertexProperties;
    vp->eq = current_element(eqIter);
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _acausalEqs->push_back(v);
    DEBUG('c', INFO, "%d: %s", vp->index, vp->eq->print().c_str());
  }

  DEBUG('c', INFO, "Unknown indexes:\n");

  index = 0;
  foreach(expIter, unknowns) {
    VertexProperties *vp = new VertexProperties;
    vp->unknown = current_element(expIter);
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _unknowns->push_back(v);
    DEBUG('c', INFO, "%d: %s\n", vp->index, vp->unknown->print().c_str());
   }

  DEBUG('c', INFO, "Graph edges:\n");

  list<Vertex>::iterator acausalEqsIter, unknownsIter;
  foreach(acausalEqsIter, _acausalEqs) {
    foreach(unknownsIter, _unknowns) {
      Vertex eqVertex = current_element(acausalEqsIter);
      Vertex unknownVertex = current_element(unknownsIter);
      if(occur(_graph[unknownVertex].unknown, _graph[eqVertex].eq)) {
        add_edge(eqVertex, unknownVertex, BLACK, _graph);
        DEBUG('c', INFO, "(%d, %d) ", _graph[eqVertex].index, _graph[unknownVertex].index);
      }
    }
  }
  DEBUG('c', INFO, "\n");

}

CausalizationStrategy::~CausalizationStrategy() {
}

MMO_EquationList CausalizationStrategy::causalize() {

  if(_acausalEqs->empty()) {
    _causalEqs1->insert(_causalEqs1->end(), _causalEqsN->begin(), _causalEqsN->end());
    return _causalEqs1;
  }

  list<Vertex>::iterator iter, auxiliaryIter;

  auxiliaryIter = _acausalEqs->begin();
  for(iter = auxiliaryIter; iter != _acausalEqs->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex eq = current_element(iter);
    Edge blackEdge;
    if(processVertex(eq, &blackEdge) == 1) {
      Vertex unknown = target(blackEdge, _graph);
      colorAdjacentEdges(unknown, blackEdge);
      _causalEqs1->push_back(_graph[eq].eq);
      _acausalEqs->erase(iter);
      _unknowns->remove(unknown);
    }
  }

  auxiliaryIter = _unknowns->begin();
  for(iter = auxiliaryIter; iter != _unknowns->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex unknown = current_element(iter);
    Edge blackEdge;
    if(processVertex(unknown, &blackEdge) == 1) {
      Vertex eq = target(blackEdge, _graph);
      colorAdjacentEdges(eq, blackEdge);
      _causalEqsN->push_back(_graph[eq].eq);
      _acausalEqs->remove(eq);
      _unknowns->erase(iter);
    }
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

int CausalizationStrategy::processVertex(Vertex v, Edge *blackEdge) {
  CausalizationGraph::out_edge_iterator ei, ei_end;
  int blackEdges = 0;
  for(boost::tie(ei, ei_end) = out_edges(v, _graph); ei != ei_end; ei++) {
    if (_graph[*ei] == BLACK) {
      ++blackEdges;
      *blackEdge = *ei;
    }
  }
  return blackEdges;
}

// TODO se puede optimizar ya que hay vertices para los cuales
// puede que ya no sea necesario colorear nada porque ya han sido
// coloreados.
void CausalizationStrategy::colorAdjacentEdges(Vertex v, Edge blackEdge) {
  CausalizationGraph::out_edge_iterator ei, ei_end;
  for(boost::tie(ei, ei_end) = out_edges(v, _graph); ei != ei_end; ei++) {
    if (*ei != blackEdge) {
      _graph[*ei] = BLUE;
    }
  }
}
