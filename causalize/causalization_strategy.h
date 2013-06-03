/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>

using namespace std;

class CausalizationStrategy {
public:
	CausalizationStrategy(MMO_EquationList equations, AST_ExpressionList unknowns);
	virtual ~CausalizationStrategy();
	MMO_EquationList causalize();
private:
	bool occur(AST_Expression unknown, MMO_Equation equation);
	MMO_EquationList _equations;
	AST_ExpressionList _unknowns;
	int _N;
	MMO_Equation *_equationByIndex;
	AST_Expression *_expressionByIndex;
};
