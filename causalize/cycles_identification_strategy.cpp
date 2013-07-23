/*
 * undirected_graph_loop_strategy.cpp
 *
 *  Created on: 12/07/2013
 *      Author: fede
 */

#include <causalize/cycles_identification_strategy.h>
#include <causalize/debug.h>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/max_cardinality_matching.hpp>
#include <boost/graph/strong_components.hpp>
#include <boost/graph/adjacency_list.hpp>

#include <assert.h>

typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS> DirectedGraph;
typedef boost::graph_traits < DirectedGraph >::vertex_descriptor DGVertex;
typedef boost::graph_traits < DirectedGraph >::edge_descriptor DGEdge;

// FIXME only works with adjacency_list class with template parameter VertexList=vecS
// because it uses integers for vertex descriptors. adjacency_list class with template parameter VertexList=listS
// won't work because it doesn't uses integers for vertex descriptors.
  std::vector<Vertex> _matching;
  std::map<DGVertex, Vertex> _collapsed2original;

void buildCollapsedGraph(CausalizationGraph& graph, DirectedGraph& digraph);
DGVertex original2collapsed(Vertex value);

// TODO ver como hacer más genérico este parámetro.
int cycles_identification_strategy(CausalizationGraph& graph, std::map<Vertex, int> *cycles) {

  bool success = checked_edmonds_maximum_cardinality_matching(graph, &_matching[0]);
  if (!success) {
    ERROR("Can't find a maximum cardinality matching.\n");
  }

  DirectedGraph collapsedGraph;
  buildCollapsedGraph(graph, collapsedGraph);

  std::map<DGVertex, int> vertex2component;
  boost::associative_property_map< std::map<DGVertex, int> >  component_map(vertex2component);
  int numCycles = strong_components(collapsedGraph, component_map);

  std::map<DGVertex, int>::iterator it;
  for (it = vertex2component.begin(); it != vertex2component.end(); it++) { // FIXME ver el caso non-cycle
    DGVertex collapsedVertex = it->first;
    Vertex eOriginalVertex = _collapsed2original[collapsedVertex];
    Vertex uOriginalVertex = _matching[eOriginalVertex];
    int component = it->second;
    (*cycles)[eOriginalVertex] = component;
    (*cycles)[uOriginalVertex] = component;
  }

  return numCycles;
}

void buildCollapsedGraph(CausalizationGraph& graph, DirectedGraph& digraph) {
  CausalizationGraph::vertex_iterator vi, vi_end;
  for(boost::tie(vi,vi_end) = vertices(graph); vi != vi_end; ++vi) {
   if (graph[*vi].type == E) {
     DGVertex v = add_vertex(digraph);
     _collapsed2original[v]=*vi;
   }
  }
  DirectedGraph::vertex_iterator vj, vj_end;
  for(boost::tie(vj,vj_end) = vertices(digraph); vj != vj_end; ++vj) {
    CausalizationGraph::out_edge_iterator ek, ek_end;
    Vertex originalVertex = _collapsed2original[*vj];
    assert(graph[originalVertex].type ==  E);
    for(boost::tie(ek,ek_end) = out_edges(originalVertex, graph); ek != ek_end; ++ek) {
      Vertex uMatchingVertex = _matching[originalVertex];
      Vertex uAdjacentVertex = target(*ek, digraph);
      if (uAdjacentVertex != uMatchingVertex) {
        Vertex eMatchingVertex = _matching[uAdjacentVertex];
        boost::add_edge(*vj, original2collapsed(eMatchingVertex), digraph);
      }
    }
  }
}

DGVertex original2collapsed(Vertex value) {
  std::map<DGVertex, Vertex>::iterator it;
  for (it = _collapsed2original.begin(); it != _collapsed2original.end(); it++){
    if ((*it).second == value) {
      return (*it).first;
    }
  }
  ERROR("Can't find collapsed vertex from original.");
}
