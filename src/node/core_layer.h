/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CCN_NODE_H
#define CCN_NODE_H

#include <omnetpp.h>

//#include "abstract_node.h"
//#include "packets/ccn_interest.h"
//#include "packets/ccn_data_m.h"
//#include "content/content_distribution.h"
//#include "core/definitions.h"
//#include <boost/unordered_map.hpp>

#include "strategy/strategy_layer.h"
#include "cache/base_cache.h"
#include "core/definitions.h"


using namespace std;
using namespace boost;


//This structure takes care of data forwarding
struct pit_entry {//new pit table
    interface_t interfaces;

    //simtime_t  timeout;
    //pit_entry (simtime_t t=0):timeout(t){;}
};


class core_layer : public AbstractNode {
    friend class statistics;

    public:
	double get_betweenness();

    protected:
    //Standard node Omnet++ functions
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

    //Custom functions
	void handle_interest(ccn_interest *);
	void handle_data(ccn_data *);
	void handle_decision(bool *, ccn_interest *);


	bool check_ownership(vector<int>);
	ccn_data *compose_data(uint64_t);	
	void clear_stat();


	
    private:
	uint32_t nodes;
	double RTT;
	double btw;
	unsigned long max_pit;

	//Architecture data structures
	unordered_map <uint64_t, pit_entry > PIT;
	base_cache *ContentStore;
	strategy_layer *strategy;

	//Statistics
	int interests;
	int data;
};
#endif

