cplusplus {{
#include "definitions.h"
}}

class noncobject chunk_t;
packet ccn_interest{
	@customize(true);
	abstract int path[];//for source routed path

	chunk_t chunk;
	int target = -1;
	short hops = 0;
	double btw = 0;
}
