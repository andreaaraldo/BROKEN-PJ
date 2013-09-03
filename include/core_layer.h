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

#include "abstract_node.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace std;
using namespace boost;

class ccn_interest;
class ccn_data;

class strategy_layer;
class base_cache;


//This structure takes care of data forwarding
struct pit_entry {
    interface_t interfaces;
    unordered_set<int> nonces;
    simtime_t time;
};


class core_layer : public AbstractNode {
    friend class statistics;

    protected:
    //Standard node Omnet++ functions
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

    //Custom functions
	void handle_interest(ccn_interest *);
	void handle_ghost(ccn_interest *);
	void handle_data(ccn_data *);
	void handle_decision(bool *, ccn_interest *);


	bool check_ownership(vector<int>);
	ccn_data *compose_data(uint64_t);	
	void clear_stat();

    private:
	unsigned long max_pit;
	unsigned short nodes;
	unsigned int my_bitmask;
	double my_btw;
	double RTT;
	static int repo_interest;
	int repo_load;
	

	//Architecture data structures
	boost::unordered_map <chunk_t, pit_entry > PIT;
	base_cache *ContentStore;
	strategy_layer *strategy;

	//Statistics
	int interests;
	int data;
};
#endif

