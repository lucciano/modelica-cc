/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */
#include <boost/lambda/lambda.hpp>

#include <ast/equation.h>

#include <causalize/causalization_strategy.h>
#include <causalize/compref_occurrence.h>
#include <causalize/cycles_identification_strategy.h>
#include <util/debug.h>
#include <util/solve.h>

CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns) {

  if (equations->size() != unknowns->size()) {
    ERROR ("The model being causalized is not balanced.\n"
           "There are %d equations and %d variables\n",
           equations->size(), unknowns->size());
  }

  int index = 0;

  _all_unknowns = unknowns;

  _eqVertices = new list<Vertex>;
  _unknownVertices = new list<Vertex>;

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
    vp->type = E;
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _eqVertices->push_back(v);
    DEBUG('c', "%d: %s", vp->index, vp->eqs->front()->print().c_str());
  }

  DEBUG('c', "Unknown indexes:\n");

  index = 0;
  foreach(expIter, unknowns) {
    VertexProperties *vp = new VertexProperties;
    vp->unknowns = newAST_ExpressionList();
    vp->unknowns->push_back(current_element(expIter));
    vp->type = U;
    vp->index = index++;
    Vertex v = add_vertex(*vp, _graph);
    _unknownVertices->push_back(v);
    DEBUG('c', "%d: %s\n", vp->index, vp->unknowns->front()->print().c_str());
   }

  DEBUG('c', "Graph edges:\n");

  list<Vertex>::iterator acausalEqsIter, unknownsIter;
  foreach(acausalEqsIter, _eqVertices) {
    foreach(unknownsIter, _unknownVertices) {
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
  delete _eqVertices;
  delete _unknownVertices;
  delete _causalEqs1;
  delete _causalEqsN;
}

void CausalizationStrategy::causalize(AST_String name, MMO_EquationList causalEqs, AST_ClassList cl) {

  if(_eqVertices->empty()) {
      causalEqs->insert(causalEqs->end(), _causalEqs1->begin(), _causalEqs1->end());
      causalEqs->insert(causalEqs->end(), _causalEqsN->begin(), _causalEqsN->end());
      return;
    }

    int acausalEqsSize = _eqVertices->size();

    list<Vertex>::iterator iter, auxiliaryIter;

    auxiliaryIter = _eqVertices->begin();
    for(iter = auxiliaryIter; iter != _eqVertices->end(); iter = auxiliaryIter) {
      ++auxiliaryIter;
      Vertex eq = current_element(iter);
      if (out_degree(eq, _graph) == 1) {
        Edge e = *out_edges(eq, _graph).first;
        Vertex unknown = target(e, _graph);
        remove_out_edge_if(unknown, boost::lambda::_1 != e, _graph);
        makeCausal1(name,_graph[eq].eqs, _graph[unknown].unknowns,cl);
        _eqVertices->erase(iter);
        _unknownVertices->remove(unknown);
      } else if (out_degree(eq, _graph) == 0) {
        ERROR("Problem is singular. Not supported yet.\n");
      }
    }

    auxiliaryIter = _unknownVertices->begin();
    for(iter = auxiliaryIter; iter != _unknownVertices->end(); iter = auxiliaryIter) {
      ++auxiliaryIter;
      Vertex unknown = current_element(iter);
      if(out_degree(unknown, _graph) == 1) {
        Edge e = *out_edges(unknown, _graph).first;
        Vertex eq = target(e, _graph);
        remove_out_edge_if(eq, boost::lambda::_1 != e, _graph);
        makeCausalN(name,_graph[eq].eqs, _graph[unknown].unknowns,cl);
        _eqVertices->remove(eq);
        _unknownVertices->erase(iter);
      } else if (out_degree(unknown, _graph) == 0) {
        ERROR("Problem is singular. Not supported yet.\n");
      }
    }

    if (acausalEqsSize == _eqVertices->size()) {
      DEBUG('c', "Algebraic loop(s) detected.\n");
      processCycles();
    }

  causalize(name, causalEqs, cl);

}

void CausalizationStrategy::makeCausal1(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl) {
  MMO_EquationListIterator iter;
  MMO_EquationList causalEqs = EquationSolver::solve(name, eqs, unknowns, _all_unknowns,cl);
  foreach(iter, causalEqs) {
    _causalEqs1->push_back(current_element(iter));
  }
}

void CausalizationStrategy::makeCausalN(AST_String name, MMO_EquationList eqs, AST_ExpressionList unknowns, AST_ClassList cl) {
  MMO_EquationListIterator iter;
  MMO_EquationList causalEqs = EquationSolver::solve(name, eqs, unknowns, _all_unknowns,cl);
  foreach(iter, causalEqs) {
    _causalEqsN->push_front(current_element(iter));
  }
}

void CausalizationStrategy::processCycles() {

  // 1) Identificamos los ciclos
  map<Vertex, int> vertex2loop;

  DEBUG('c', "Identifing cycles...\n");

  int num_cycles = cycles_identification_strategy(_graph, vertex2loop);

  DEBUG('c', "%d cycles identified\n", num_cycles);

  for (map<Vertex,int>::iterator it=vertex2loop.begin(); it!=vertex2loop.end(); ++it) {
    Vertex v = it->first;
    DEBUG('c', "Vertex: %d -- Cycle: %d\n", _graph[v].index, it->second);
  }

  // 2) Preparamos los nuevos vertices que van a agrupar los conjuntos de ecuaciones pertenecientes a un mismo ciclo.
  //    Idem para las incógnitas.
  vector<Vertex> newEVertexVector(num_cycles);
  vector<Vertex> newUVertexVector(num_cycles);
  for (int i = 0; i<num_cycles; i++) {
    VertexProperties *vp = new VertexProperties;
    vp->eqs = newMMO_EquationList();
    vp->type = E;
    // TODO vp->index
    Vertex eVertex = add_vertex(*vp, _graph);
    newEVertexVector[i] = eVertex;
    vp = new VertexProperties;
    vp->unknowns = newAST_ExpressionList();
    vp->type = U;
    // TODO vp->index
    Vertex uVertex = add_vertex(*vp, _graph);
    newUVertexVector[i] = uVertex;
  }

  // 3) Populamos los vertices creados en el punto anterior.
  list<Vertex>::iterator iter = _eqVertices->begin();
  foreach(iter, _eqVertices) {
    Vertex oldEVertex = current_element(iter);
    int cycle = vertex2loop[oldEVertex];
    if (cycle != NONE_CYCLE) {
      Vertex newEVertex = newEVertexVector[cycle];
      MMO_EquationList oldEqs = _graph[oldEVertex].eqs;
      MMO_EquationList newEqs = _graph[newEVertex].eqs;
      newEqs->insert(newEqs->end(), oldEqs->begin(), oldEqs->end());
    }
  }
  iter = _unknownVertices->begin();
  foreach(iter, _unknownVertices) {
    Vertex oldUVertex = current_element(iter);
    int cycle = vertex2loop[oldUVertex];
    if (cycle != NONE_CYCLE) {
      Vertex newUVertex = newUVertexVector[cycle];
      AST_ExpressionList oldUnknowns = _graph[oldUVertex].unknowns;
      AST_ExpressionList newUnknowns = _graph[newUVertex].unknowns;
      newUnknowns->insert(newUnknowns->end(), oldUnknowns->begin(), oldUnknowns->end());
    }
  }

  // 4) Trazamos las nuevas aristas y eliminamos los vértices viejos que fueron
  // sustituidos por los macro-vertices.
  for (int i = 0; i<num_cycles; i++) {
    add_edge(newEVertexVector[i], newUVertexVector[i], _graph);
  }

  iter = _eqVertices->begin();
  foreach(iter, _eqVertices) {
    Vertex eVertex = current_element(iter);
    if (vertex2loop[eVertex] == NONE_CYCLE) {
      CausalizationGraph::out_edge_iterator ei, eiend, next;
      boost::tie(ei, eiend) = out_edges(eVertex, _graph);
      for (next = ei; ei != eiend; ei = next) {
        next++;
        Vertex uVertex = target(*ei, _graph);
        int uLoop = vertex2loop[uVertex];
        if (uLoop != NONE_CYCLE) {
          add_edge(eVertex, newUVertexVector[uLoop], _graph);
          remove_edge(ei, _graph);
          _unknownVertices->remove(uVertex);
        }
      }
    }
  }

  list<Vertex>::iterator auxiliaryIter = _unknownVertices->begin();
  for(iter = auxiliaryIter; iter != _unknownVertices->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex uVertex = current_element(iter);
    if (vertex2loop[uVertex] != NONE_CYCLE) {
      _unknownVertices->erase(iter);
    }
  }

  iter = _unknownVertices->begin();
  foreach(iter, _unknownVertices) {
    Vertex uVertex = current_element(iter);
    if (vertex2loop[uVertex] == NONE_CYCLE) {
      CausalizationGraph::out_edge_iterator vi, viend, next;
      boost::tie(vi, viend) = out_edges(uVertex, _graph);
      for (next = vi; vi != viend; vi = next) {
        next++;
        Vertex eVertex = target(*vi, _graph);
        int eCycle = vertex2loop[eVertex];
        if (eCycle != NONE_CYCLE) {
          add_edge(uVertex, newEVertexVector[eCycle], _graph);
          remove_edge(vi, _graph);
          _eqVertices->remove(eVertex);
        }
      }
    }
  }

  auxiliaryIter = _eqVertices->begin();
  for(iter = auxiliaryIter; iter != _eqVertices->end(); iter = auxiliaryIter) {
    ++auxiliaryIter;
    Vertex eVertex = current_element(iter);
    if (vertex2loop[eVertex] != NONE_CYCLE) {
      _eqVertices->erase(iter);
    }
  }

  // 5) Agregamos los macro-vertices a la lista de ecuaciones por causalizar.
  for (int i = 0; i < num_cycles; i++) {
    _eqVertices->push_back(newEVertexVector[i]);
  }

  for (int i = 0; i < num_cycles; i++) {
    _unknownVertices->push_back(newUVertexVector[i]);
  }

}
