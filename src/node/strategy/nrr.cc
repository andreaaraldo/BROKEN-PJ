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
#include <omnetpp.h>
#include <algorithm>
#include "nrr.h"
#include "ccnsim.h"
#include "ccn_interest.h"
#include "base_cache.h"
#include "error_handling.h"

Register_Class(nrr);

struct lookup{
    chunk_t elem;
    lookup(chunk_t e):elem(e){;}
    bool operator() (Centry c) const { return c.cache->fake_lookup(elem); }
};

struct lookup_len{
    chunk_t elem;
    int len;
    lookup_len(chunk_t e,int l):elem(e),len(l){;}
    bool operator() (Centry c) const { return c.cache->fake_lookup(elem) && c.len ==len; }
};


void nrr::initialize(){
    strategy_layer::initialize();
    vector<string> ctype;
    ctype.push_back("modules.node.node");
    TTL = par("TTL2");

    cTopology topo;
    topo.extractByNedTypeName(ctype);
    for (int i = 0;i<topo.getNumNodes();i++){
	if (i==getIndex()) continue;
	base_cache *cptr = (base_cache *)topo.getNode(i)->getModule()->getModuleByRelativePath("content_store");
	//<aa>
	const int_f FIB_entry = get_FIB_entry(i);
	if (FIB_entry.len <= TTL)
	    cfib.push_back( Centry ( cptr, FIB_entry.len ) );
    }	
    //Commented the following if block
	//</aa>
	/**
	if (FIB[i].len <= TTL)
	    cfib.push_back( Centry ( cptr, FIB[i].len ) );
    }
    */
    
    sort(cfib.begin(), cfib.end());
}

bool *nrr::get_decision(cMessage *in){

    bool *decision;
    if (in->getKind() == CCN_I){
	ccn_interest *interest = (ccn_interest *)in;
	decision = exploit(interest);
    }
    return decision;

}



//The nearest repository just exploit the host-centric FIB. 
bool *nrr::exploit(ccn_interest *interest){

    int repository,
	node,
	output_iface,
	gsize,
	times;

	output_iface = -1;

    gsize = __get_outer_interfaces();
    bool *decision = new bool[gsize];
    std::fill(decision,decision+gsize,0);


	#ifdef SEVERE_DEBUG
	if (interest->getTarget() == getIndex() ){
		std::stringstream ermsg; 
		ermsg<<"I am node "<<getIndex()<<".I am the target for  an interest for chunk " 			<< interest->getChunk() <<" but I do not have that chunk";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	#endif

    //find the first occurrence in the sorted vector of caches.
    if (interest->getTarget() == -1 
		//<aa> 	The interest has no target node () the preferential node to be sent 
		// 		to</aa>
		|| interest->getTarget() == getIndex() 
		// <aa> The target of the interest is this node </aa>

	){
		vector<Centry>::iterator it = 
			std::find_if (cfib.begin(),cfib.end(),lookup(interest->getChunk()) );

		vector<int> repos = interest->get_repos();
		repository = nearest(repos);

		//<aa>
		const int_f FIB_entry = get_FIB_entry(repository);
		//</aa>
		if (it!=cfib.end() && it->len <= FIB_entry.len+1)
		{//found!!!
			//<aa>	It is possible to reach the content through the interface indicated
			//		by it. Moreover, this path is not longer than the path related to
			//		the FIB_entry </aa>

			times = std::count_if (
					cfib.begin(),cfib.end(),lookup_len(interest->getChunk(),it->len));
			int select = intrand(times);
			it+=select;
			node = it->cache->getIndex();
			output_iface = FIB_entry.id;
			interest->setTarget(node);
		}else{//not found
			//<aa> There are no alternatives to the FIB entry to reach the content</aa>
			output_iface = FIB_entry.id;
			interest->setTarget(repository);
		}

    }else {
		//<aa>	The interest has already a target that is not this node. 
		// I will send the
		//		interest toward that target </aa>
		const int_f FIB_entry2 = get_FIB_entry(interest->getTarget() );
		output_iface = FIB_entry2.id;
    }

	#ifdef SEVERE_DEBUG
	if ( output_iface == -1 ){
		std::stringstream ermsg; 
		ermsg<<"I am node "<<getIndex()<<".The output iface for interest for chunk "
			<< interest->getChunk() <<" was not satisfied";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	#endif

    decision[output_iface] = true;
    return decision;
}

int nrr::nearest(vector<int>& repositories){
    int  min_len = 10000;
    vector<int> targets
	//<aa>
			(0);
	//</aa>

    for (vector<int>::iterator i = repositories.begin(); i!=repositories.end();i++){ 		//Find the shortest (the minimum)
    	//<aa>
    	const int_f FIB_entry = get_FIB_entry(*i);
    	//</aa>
        if (FIB_entry.len < min_len ){
            min_len = FIB_entry.len;
            targets.clear();
            targets.push_back(*i);
        }else if (FIB_entry.len == min_len)
		    targets.push_back(*i);
    }

	//<aa>
	#ifdef SEVERE_DEBUG
	if (repositories.size() == 0){
		std::stringstream ermsg; 
		ermsg<<"There are 0 repositories. It's not admitted";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	int test = targets[0]; // Just to see if targets is properly allocated
	#endif
	//</aa>

    return targets[intrand(targets.size())];
}

void nrr::finish(){
    ;
    //string id = "nodegetIndex()+"]";
}

