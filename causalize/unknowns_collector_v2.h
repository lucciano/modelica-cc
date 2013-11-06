/*
 * unknowns_collector_v2.h
 *
 *  Created on: 28/10/2013
 *      Author: fede
 */

#ifndef UNKNOWNS_COLLECTOR_V2_H_
#define UNKNOWNS_COLLECTOR_V2_H_

#include <mmo/mmo_class.h>
#include <util/ast_util.h>
#include <util/symbol_table.h>


class UnknownsCollectorV2: public AST_Expression_Traverse {
public:
  UnknownsCollectorV2(MMO_Class mmo_class);
  AST_ExpressionList collectUnknowns();
private:
  virtual AST_Expression mapTraverseElement(AST_Expression);
  VarInfo getVarInfo(AST_Expression_ComponentReference compRef);
  MMO_Class _mmo_class;
  VarSymbolTable _varSymbolTable;
  AST_ExpressionList _unknowns;
};

#endif /* UNKNOWNS_COLLECTOR_V2_H_ */
