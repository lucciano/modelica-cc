/*
 * ComponentReferenceOcurrenceTraverse.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <util/ast_util.h>
#include <mmo/mmo_class.h>

using namespace std;

bool occur(AST_Expression unknown, MMO_Equation equation);

class CompRefOccurrenceTraverse: public AST_Expression_Fold<bool>  {
public:
	CompRefOccurrenceTraverse(AST_Expression unknown);
	virtual ~CompRefOccurrenceTraverse();
private:
	virtual bool foldTraverseElementUMinus(AST_Expression exp);
	virtual bool foldTraverseElement(AST_Expression equation);
	virtual bool foldTraverseElement(bool , bool , BinOpType);
	AST_Expression _unknown;
};
