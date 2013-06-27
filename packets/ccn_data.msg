cplusplus {{
#include "ccnsim.h"
}}

class noncobject chunk_t;
packet ccn_data{
	@customize(true);
	chunk_t chunk;

	int target = -1;
	int origin = -1;
	int hops = 0;

	int TSB = 0;
	int TSI = 0;
	int capacity;

	double btw = 0;
	bool ghost = 0;
	bool found = 0;
}
