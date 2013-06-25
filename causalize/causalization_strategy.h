/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace std;

struct VertexProperties {
  MMO_EquationList eqs;
  AST_ExpressionList unknowns;
  int index; // only for debugging purposes
};

typedef boost::adjacency_list<boost::listS,
        boost::listS, boost::undirectedS, VertexProperties> CausalizationGraph;
typedef CausalizationGraph::vertex_descriptor Vertex;
typedef CausalizationGraph::edge_descriptor Edge;

class CausalizationStrategy {
public:
	CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns);
	virtual ~CausalizationStrategy();
	MMO_EquationList causalize();
private:
	bool occur(AST_Expression unknown, MMO_Equation equation);
	void makeCausal1(MMO_EquationList eqs, AST_ExpressionList unknowns);
	void makeCausalN(MMO_EquationList eqs, AST_ExpressionList unknowns);
//	int processVertex(Vertex v, Edge *blackEdge);
//	void colorAdjacentEdges(Vertex v, Edge blackEdge);
	CausalizationGraph _graph;
	list<Vertex> *_acausalEqs;
	list<Vertex> *_unknowns;
	MMO_EquationList _causalEqs1; // equations numbered starting from 1
	MMO_EquationList _causalEqsN; // equations numbered starting from n
};
