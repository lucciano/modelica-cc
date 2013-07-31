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

map<Vertex, Vertex> _matching;
map<DGVertex, Vertex> _collapsed2original;

void buildCollapsedGraph(CausalizationGraph& graph, DirectedGraph& digraph);

void process_strong_components_result(map<DGVertex, int> *vertex2component, int *numComponents);

int cycles_identification_strategy(CausalizationGraph& graph, map<Vertex, int> *cycles) {

  boost::associative_property_map< map<Vertex, Vertex> >  matching_map(_matching);
  map<Vertex, int> vertex2index;
  CausalizationGraph::vertex_iterator i, iend;
  int ic = 0;
  for (boost::tie(i, iend) = vertices(graph); i != iend; ++i, ++ic) {
    vertex2index[*i] = ic;
  }
  boost::associative_property_map< map<Vertex, int> >  index_map(vertex2index);
  bool success = checked_edmonds_maximum_cardinality_matching(graph, matching_map, index_map);
  if (!success) {
    ERROR("Can't find a maximum cardinality matching.\n");
  }

  DirectedGraph collapsedGraph;
  buildCollapsedGraph(graph, collapsedGraph);

  map<DGVertex, int> vertex2component;
  boost::associative_property_map< map<DGVertex, int> >  component_map(vertex2component);
  map<DGVertex, int> dg_vertex2index;
  DirectedGraph::vertex_iterator j, jend;
  int jc = 0;
  for (boost::tie(j, jend) = vertices(collapsedGraph); j != jend; ++j, ++jc) {
    dg_vertex2index[*j] = jc;
  }
  boost::associative_property_map< map<DGVertex, int> >  dg_index_map(dg_vertex2index);
  int numCycles = strong_components(collapsedGraph, component_map, boost::vertex_index_map(dg_index_map));

  process_strong_components_result(&vertex2component, &numCycles);

  map<DGVertex, int>::iterator it;
  for (it = vertex2component.begin(); it != vertex2component.end(); it++) {
    DGVertex collapsedVertex = it->first;
    Vertex eOriginalVertex = _collapsed2original[collapsedVertex];
    Vertex uOriginalVertex = _matching[eOriginalVertex];
    int component = it->second;
    (*cycles)[eOriginalVertex] = component;
    (*cycles)[uOriginalVertex] = component;
  }

  return numCycles;
}

DGVertex original2collapsed(Vertex value);

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
  map<DGVertex, Vertex>::iterator it;
  for (it = _collapsed2original.begin(); it != _collapsed2original.end(); it++){
    if ((*it).second == value) {
      return (*it).first;
    }
  }
  ERROR("Can't find collapsed vertex from original.");
}

void set_cycle_index(map<DGVertex, int> *vertex2component, int comp, int index);

// filter strong components of size 1
void process_strong_components_result(map<DGVertex, int> *vertex2component, int *numComponents) {
  vector<int> compSize(*numComponents, 0);
  map<DGVertex, int>::iterator it;
  for (it = vertex2component->begin(); it != vertex2component->end(); ++it) {
    compSize[(*it).second]++;
  }
  int cycleIndex = 0;
  for (vector<int>::size_type i = 0; i < compSize.size(); ++i) {
    if (compSize[i] == 1) {
      (*numComponents)--;
      set_cycle_index(vertex2component, i, NONE_CYCLE);
    } else {
      set_cycle_index(vertex2component, i, cycleIndex++);
    }
  }
}

void set_cycle_index(map<DGVertex, int> *vertex2component, int comp, int index) {
  map<DGVertex, int>::iterator it;
  for (it = vertex2component->begin(); it != vertex2component->end(); it++) {
    if ((*it).second == comp) {
      (*it).second = index;
    }
  }
}
