//<aa>
#ifndef PROACTIVE_COMPONENT_H_
#define PROACTIVE_COMPONENT_H_

#include "ccnsim.h"
#include "client.h"
#include "error_handling.h"

using namespace std;

class ProactiveComponent:public client
{
    public:
		virtual void initialize();
		void try_to_improve(chunk_t stored, chunk_t requested_chunk_id);

};
#endif
//</aa>
