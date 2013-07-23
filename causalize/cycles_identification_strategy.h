/*
 * undirected_graph_loop_strategy.h
 *
 *  Created on: 12/07/2013
 *      Author: fede
 */

#include <causalize/graph_definition.h>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

#define NON_CYCLE -1

/**
 * Implements an algorithm to identify all the cycles in a causalization graph.
 * @param[in] graph The graph where to look for the cycles.
 * @param[out] cycles A vector containing the cycles.
 * @return The amount of cycles detected in the graph.
 */
int cycles_identification_strategy(CausalizationGraph& graph, std::map<Vertex, int> *cycles);

