cplusplus {{
#include "ccnsim.h"
}}

class noncobject chunk_t;
packet ccn_interest{
	@customize(true);
	abstract int path[];//for source routed path (@deprecated)

	chunk_t chunk;
	int nonce = 0;
	int hops = 0;


	int target = -1; //generic target (can be a repository or a generic node)
	int rep_target = -1; //repository target (should be set to a repository)
	double btw = 0;

	int capacity = 0;;

	bool ghost = 0;
	bool found = 0;
	int origin = -1;
	int TTL;
}
