cplusplus {{
#include "definitions.h"
}}

class noncobject chunk_t;
packet ccn_data{
	chunk_t chunk;
	@customize(true);

	int target = -1;
	short hops = 0;
	double btw = 0;
}
