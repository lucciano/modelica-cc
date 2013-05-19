
#include <mmo/mmo_class.h>

using namespace std;

class UnknownCollector {
	public:
		UnknownCollector(MMO_Class *c);
		virtual ~UnknownCollector();
		list<string> *collectUnknowns();

	private:
		string replaceStateByDer(string state);
		MMO_Class *_c;
		list<string> *_unknowns;
};
