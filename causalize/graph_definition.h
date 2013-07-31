#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>

#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>

enum VertexType { E, // Equations
                  U  // Unknowns
};

struct VertexProperties {
  VertexType type;
  MMO_EquationList eqs;
  AST_ExpressionList unknowns;
  int index; // only for debugging purposes
};

typedef boost::adjacency_list<boost::listS,
        boost::listS, boost::undirectedS, VertexProperties> CausalizationGraph;

typedef CausalizationGraph::vertex_descriptor Vertex;

typedef CausalizationGraph::edge_descriptor Edge;
