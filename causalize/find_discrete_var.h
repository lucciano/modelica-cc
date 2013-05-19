/*
 * find_discrete_var.h
 *
 *  Created on: 16/05/2013
 *      Author: fede
 */

#ifndef FIND_DISCRETE_VAR_H_
#define FIND_DISCRETE_VAR_H_

#include<mmo/mmo_class.h>
#include<util/symbol_table.h>

class DiscreteVariablesFinder {
public:
  DiscreteVariablesFinder(MMO_Class *c);
  virtual ~DiscreteVariablesFinder();
  void findDiscreteVariables();
private:
  void findDiscreteVariablesInEquations(MMO_EquationList eqs);
  MMO_Class *_c;
  VarSymbolTable _varSymbolTable;
};

#endif /* FIND_DISCRETE_VAR_H_ */
