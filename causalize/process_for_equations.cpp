#include <causalize/process_for_equations.h>

RangeIterator* processInExp(AST_Expression inExp){
  switch (inExp->expressionType()) {
  case EXPRANGE:
    // TODO
    break;
  default:
    ERROR("process_for_equations:\n"
        "Equation type not supported in forIndex inExp\n");
  }
}

void process_for_equations(MMO_EquationList equations, AST_ExpressionList unknowns) {

  MMO_EquationListIterator eqIter;
  AST_ExpressionListIterator expIter;

  foreach(eqIter, equations) {
    MMO_Equation eq = current_element(eqIter);
    if (eq->equationType() == EQFOR) {
      AST_Equation_For forEq = (AST_Equation_For) eq->getAsFor();
      AST_ForIndexList forIndexList = forEq->forIndexList();
      ERROR_UNLESS(forIndexList != NULL,
          "process_for_equations:\n"
          "forIndexList is NULL\n");
      ERROR_UNLESS(!forIndexList->empty(),
                "process_for_equations:\n"
                "forIndexList is Empty\n");
      ERROR_UNLESS(forIndexList->size() == 1,
                      "process_for_equations:\n"
                      "forIndexList with more than 1 forIndex are not supported\n");
      AST_ForIndex forIndex = forIndexList->front();
      AST_Expression inExp = forIndex->in_exp();
      if (inExp != NULL) {
        RangeIterator* rangeIter = processInExp(inExp);
        while (rangeIter->hasNext()) {
          // E value
          // TODO Crear las eqs nuevas
          // sustituyendo los subindices
          // de los arrays por value
        }
      } else {
        // TODO [Moya] Implicit range
      }
    }
  }

}

template <class E>
class RangeIterator {
public:
  virtual bool hasNext();
  virtual E next();
};
