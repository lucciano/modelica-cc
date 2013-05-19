
#include <mmo/mmo_class.h>

class UnknownCollector {
	public:
		UnknownCollector(MMO_Class *c);
		virtual ~UnknownCollector();
		void collectUnknowns();

	private:
		MMO_Class *_c;
};
