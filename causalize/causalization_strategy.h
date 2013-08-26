/*
 * causalization_strategy.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <causalize/graph_definition.h>

using namespace std;

AST_ClassList causalize(AST_String name, MMO_EquationList equations, AST_ExpressionList unknowns, MMO_EquationList causalEqs);
bool occur(AST_Expression unknown, MMO_Equation equation);
