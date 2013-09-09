#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>

/**
 * Expande las ecuaciones de tipo "for" que haya en equations. El conjunto de ecuaciones resultante
 * lo añade a la lista equations. Las variables de tipo arreglo también son expandidas
 * y agregadas a la lista unknowns.
 */
void process_for_equations(MMO_EquationList equations, AST_ExpressionList unknowns);
