/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/graph_definition.h>

using namespace std;

class CausalizationStrategy {
public:
	CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns);
	virtual ~CausalizationStrategy();
	MMO_EquationList causalize();
private:
	bool occur(AST_Expression unknown, MMO_Equation equation);
	void makeCausal1(MMO_EquationList eqs, AST_ExpressionList unknowns);
	void makeCausalN(MMO_EquationList eqs, AST_ExpressionList unknowns);
	void processLoops();
	CausalizationGraph _graph;
	list<Vertex> *_acausalEqs;
	list<Vertex> *_unknowns;
	MMO_EquationList _causalEqs1; // equations numbered starting from 1
	MMO_EquationList _causalEqsN; // equations numbered starting from n
};
