
#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>
#include <causalize/state_variables_finder.h>

using namespace std;

class UnknownsCollector {
	public:
		UnknownsCollector(MMO_Class c);
		virtual ~UnknownsCollector();
		AST_ExpressionList collectUnknowns();
	private:
		int getDimension(AST_Expression arrayDimension, VarSymbolTable symbolTable);
		int getCompRefVal(AST_Expression_ComponentReference compRef, VarSymbolTable symbolTable);
		AST_Expression buildArray(AST_String name, int index);
		MMO_Class _c;
		AST_ExpressionList _unknowns;
		StateVariablesFinder *_finder;
};
