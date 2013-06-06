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

enum EdgeColor {
  BLACK,
  BLUE
};

struct VertexProperties {
  MMO_Equation eq;
  AST_Expression unknown;
};

typedef boost::adjacency_list<boost::listS, boost::listS, boost::undirectedS,
    VertexProperties, EdgeColor> CausalizationGraph;
typedef CausalizationGraph::vertex_descriptor Vertex;
typedef CausalizationGraph::edge_descriptor Edge;

class CausalizationStrategy {
public:
	CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns);
	virtual ~CausalizationStrategy();
	MMO_EquationList causalize();
private:
	bool occur(AST_Expression unknown, MMO_Equation equation);
	CausalizationGraph _graph;
	list<Vertex> *_eqVertices;
	list<Vertex>::iterator _eqVerticesIter;
	list<Vertex> *_unknownVertices;
	list<Vertex>::iterator _unknownVerticesIter;
//
//	MMO_EquationList _equations;
//	AST_ExpressionList _unknowns;
//	int _N;
//	MMO_Equation *_equationByIndex;
//	AST_Expression *_expressionByIndex;
};
