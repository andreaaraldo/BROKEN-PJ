cplusplus {{
#include "definitions.h"
}}

class noncobject chunk_t;
packet ccn_interest{
	@customize(true);
	abstract int path[];//for source routed path

	chunk_t chunk;
	int target = -1; //generic target (can be a repository or a generic node)
	int rep_target = -1; //repository target (should be set to a repository)
	short hops = 0;
	double btw = 0;

}
