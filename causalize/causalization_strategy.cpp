/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <ast/equation.h>
#include <util/solve.h>

#include <causalize/causalization_strategy.h>
#include <causalize/compref_occurrence_traverse.h>
#include <causalize/debug.h>

#include <boost/lambda/lambda.hpp>


CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations,
		AST_ExpressionList unknowns) {

	if (equations->size() != unknowns->size()) {
	  ERROR ("The model being causalized is not balanced.\nThere are %d equations and %d variables\n",equations->size(),unknowns->size()); // TODO No deberiamos lanzar una excepción acá?
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
    vp->eqs = newMMO_EquationList();
    vp->eqs->push_back(current_element(eqIter));
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _acausalEqs->push_back(v);
    DEBUG('c', "%d: %s", vp->index, vp->eqs->front()->print().c_str());
  }

  DEBUG('c', "Unknown indexes:\n");

  index = 0;
  foreach(expIter, unknowns) {
    VertexProperties *vp = new VertexProperties;
    vp->unknowns = newAST_ExpressionList();
    vp->unknowns->push_back(current_element(expIter));
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _unknowns->push_back(v);
    DEBUG('c', "%d: %s\n", vp->index, vp->unknowns->front()->print().c_str());
   }

  DEBUG('c', "Graph edges:\n");

  list<Vertex>::iterator acausalEqsIter, unknownsIter;
  foreach(acausalEqsIter, _acausalEqs) {
    foreach(unknownsIter, _unknowns) {
      Vertex eqVertex = current_element(acausalEqsIter);
      Vertex unknownVertex = current_element(unknownsIter);
      if(occur(_graph[unknownVertex].unknowns->front(), _graph[eqVertex].eqs->front())) {
        add_edge(eqVertex, unknownVertex, _graph);
        DEBUG('c', "(%d, %d) ", _graph[eqVertex].index, _graph[unknownVertex].index);
      }
    }
  }

  DEBUG('c', "\n");

}

CausalizationStrategy::~CausalizationStrategy() {
  delete _acausalEqs;
  delete _unknowns;
  delete _causalEqs1;
  delete _causalEqsN;
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
      makeCausal1(_graph[eq].eqs, _graph[unknown].unknowns);
      _acausalEqs->erase(iter);
      _unknowns->remove(unknown);
    } else if (out_degree(eq, _graph) == 0) {
      ERROR("Problem is singular.Not supported yet.\n");
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
      makeCausalN(_graph[eq].eqs, _graph[unknown].unknowns);
      _acausalEqs->remove(eq);
      _unknowns->erase(iter);
    } else if (out_degree(unknown, _graph) == 0) {
      ERROR("Problem is singular.Not supported yet.\n");
    } 
  }

  if (acausalEqsSize == _acausalEqs->size()) {
    ERROR("Algebraic loop(s) detected and not supported yet.\n");
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

void CausalizationStrategy::makeCausal1(MMO_EquationList eqs, AST_ExpressionList unknowns) {
  MMO_EquationListIterator iter;
  MMO_EquationList causalEqs = EquationSolver::solve(eqs, unknowns);
  foreach(iter, causalEqs) {
    _causalEqs1->push_back(current_element(iter));
  }
}

void CausalizationStrategy::makeCausalN(MMO_EquationList eqs, AST_ExpressionList unknowns) {
  MMO_EquationListIterator iter;
  MMO_EquationList causalEqs = EquationSolver::solve(eqs, unknowns);
  foreach(iter, causalEqs) {
    _causalEqsN->push_front(current_element(iter));
  }
}

//void CausalizationStrategy::processLoops() {
//
//  // 1) Identificamos los componentes conexos
//  unordered_map<Vertex, int> *vertex2loop = new unordered_map<Vertex, int>;
//  int num = find_loops(G, vertex2loop);
//
//  // 2) Preparamos los nuevos vertices que van a agrupar los conjuntos de ecuaciones pertenecientes a un mismo componente conexo.
//  //    Idem para las incógnitas.
//  vector<Vertex> newEVertexVector(num);
//  vector<Vertex> newUVertexVector(num);
//  for (int i = 0; i<num; i++) {
//    VertexProperties *vp = new VertexProperties;
//    vp->eqs = newMMO_EquationList();
//    Vertex eVertex = add_vertex(*vp, _graph);
//    newEVertexVector[i] = eVertex;
//    vp = new VertexProperties;
//    vp->unknowns = newAST_ExpressionList();
//    Vertex uVertex = add_vertex(*vp, _graph);
//    newUVertexVector[i] = uVertex;
//  }
//
//  // 3) Populamos los vertices creados en el punto anterior.
//  list<Vertex>::iterator iter = _acausalEqs->begin();
//  foreach(iter, _acausalEqs) {
//    Vertex oldEVertex = current_element(iter);
//    int component = vertex2loop->at(oldEVertex);
//    Vertex newEVertex = newEVertexVector[component];
//    MMO_EquationList oldEqs = _graph[oldEVertex].eqs;
//    MMO_EquationList newEqs = _graph[newEVertex].eqs;
//    newEqs->insert(newEqs->end(), oldEqs->begin(), oldEqs->end());
//  }
//  iter = _unknowns->begin();
//  foreach(iter, _unknowns) {
//    Vertex oldUVertex = current_element(iter);
//    int component = vertex2loop->at(oldUVertex);
//    Vertex newEVertex = newEVertexVector[component];
//    AST_ExpressionList oldUnknowns = _graph[oldUVertex].unknowns;
//    AST_ExpressionList newUnknowns = _graph[newEVertex].unknowns;
//    newUnknowns->insert(newUnknowns->end(), oldUnknowns->begin(), oldUnknowns->end());
//  }
//
//  // 4) Trazamos las aristas entre los nuevos vértices y eliminamos los vértices viejos.
////  for (int i=0; i<num; i++) {
////    add_edge(newEVertexVector[i], newUVertexVector[i], _graph);
////    list<Vertex>::iterator iter = _acausalEqs->begin();
////    foreach(iter, _acausalEqs) {
////      Vertex eVertex = current_element(iter);
////      if (vertex2loop->at(eVertex) != i) {
////        pair<Edge, bool> p = edge(, newUVertexVector[i]);
////
////      }
////    }
////  }
//
//  delete vertex2loop;
//}
