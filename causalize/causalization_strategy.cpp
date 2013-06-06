/*
 * causalization_strategy.cpp
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/causalization_strategy.h>
#include <ast/equation.h>
#include <causalize/compref_occurrence_traverse.h>

#include <iostream>

CausalizationStrategy::CausalizationStrategy(MMO_EquationList equations,
		AST_ExpressionList unknowns) {

	// TODO [Moya] Verificar #equations = # unknowns

  _eqVertices = new list<Vertex>;
  _unknownVertices = new list<Vertex>;

  MMO_EquationListIterator eqIter;
  AST_ExpressionListIterator expIter;

  foreach(eqIter, equations) {
    VertexProperties *vp = new VertexProperties;
    vp->eq = current_element(eqIter);
    Vertex v = add_vertex(*vp, _graph);
    _eqVertices->push_back(v);
  }

  foreach(expIter, unknowns) {
    VertexProperties *vp = new VertexProperties;
    vp->unknown = current_element(expIter);
    Vertex v = add_vertex(*vp, _graph);
    _unknownVertices->push_back(v);
   }

  foreach(_eqVerticesIter, _eqVertices) {
    foreach(_unknownVerticesIter, _unknownVertices) {
      Vertex eqVertex = current_element(_eqVerticesIter);
      Vertex unknownVertex = current_element(_unknownVerticesIter);
      if(occur(_graph[unknownVertex].unknown, _graph[eqVertex].eq)) {
        add_edge(eqVertex, unknownVertex, BLACK, _graph);
      }
    }
  }

//  _equations = equations;
//	_unknowns = unknowns;

//	_N = equations->size();
//	_equationByIndex = new MMO_Equation[_N];
//	MMO_EquationListIterator eqIter;
//	int i = 0;
//	foreach(eqIter, equations) {
//		MMO_Equation eq = current_element(eqIter);
//		_equationByIndex[i++] = eq;
//	}
//	_expressionByIndex = new AST_Expression[_N];
//	AST_ExpressionListIterator expIter;
//	  int j = 0;
//	  foreach(expIter, _unknowns) {
//	    AST_Expression exp = current_element(expIter);
//	    _expressionByIndex[j++] = exp;
//	}
}

CausalizationStrategy::~CausalizationStrategy() {
//	delete [] _equationByIndex;
}

MMO_EquationList CausalizationStrategy::causalize() {
  CausalizationGraph::edge_iterator ei, ei_end;
  for (tie(ei, ei_end) = edges(_graph); ei != ei_end; ++ei) {
    cout << _graph[source(*ei, _graph)].eq << " - " << _graph[target(*ei, _graph)].unknown << endl;
  }

//	AST_ExpressionListIterator expIter;
//  foreach(expIter, _unknowns) {
//		AST_Expression unknown = current_element(expIter);
//		for(int i=0; i<_N; i++){
//			if (occur(unknown, _equationByIndex[i])){
//				cout << _equationByIndex[i] << " " << unknown << endl;
//			}
//		}
//	}
  return NULL;
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
