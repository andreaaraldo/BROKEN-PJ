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
#ifndef B_CACHE_H_
#define B_CACHE_H_


#include "node/abstract_node.h"
class DecisionPolicy;



//Base cache class: it implements the basic behaviour of every cache by the mean of two abstract functions:
//
//-) storeData: stores chunks within the cache with a given policy
//-) lookup: return if the given chunk exists within the cache
//

struct cache_stat_entry{
    uint32_t miss;
    uint32_t hit;
    cache_stat_entry():miss(0),hit(0){;}
    double rate(){ return hit *1./(hit+miss);}
};

class base_cache : public AbstractNode {
    friend class statistics;
    protected:


	virtual void initialize();
	virtual void handleMessage (cMessage *){;}
	virtual void finish();
	



    public:
	//Outside function behaviour
	void clear_stat();
	bool lookup(uint64_t);
	void received_data (cMessage *);
	uint32_t get_size() { return cache_size; }

	//Inteface function (depending by internal data structures of each cache)
	virtual void store (uint64_t) = 0; 
	virtual bool data_lookup(uint64_t)=0;
	virtual bool full()=0;


    private:
	uint32_t cache_size;
	uint32_t nodes;

	DecisionPolicy *decisor;

	//Average statistics
	uint32_t miss;
	uint32_t hit;


	//Per file statistics
	cache_stat_entry *cache_stats;
};

#endif
