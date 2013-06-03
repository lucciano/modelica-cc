/*
 * ComponentReferenceOcurrenceTraverse.h
 *
 *  Created on: 12/05/2013
 *      Author: fede
 */

#include <util/ast_util.h>

using namespace std;

class CompRefOccurrenceTraverse: public AST_Expression_Fold<bool>  {
public:
	CompRefOccurrenceTraverse(AST_Expression unknown);
	virtual ~CompRefOccurrenceTraverse();
private:
	virtual bool foldTraverseElement(AST_Expression equation);
	virtual bool foldTraverseElement(bool , bool , BinOpType);
	AST_Expression _unknown;
};
