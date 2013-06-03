
#include <ast/ast_builder.h>
#include <mmo/mmo_class.h>

using namespace std;

class UnknownsCollector {
	public:
		UnknownsCollector(MMO_Class c);
		virtual ~UnknownsCollector();
		AST_ExpressionList collectUnknowns();

	private:
		string replaceStateByDer(string state);
		MMO_Class _c;
		AST_ExpressionList _unknowns;
};
