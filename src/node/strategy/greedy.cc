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
#include "greedy.h"
#include "ccnsim.h"
#include "ccn_interest.h"
#include "base_cache.h"

Register_Class(greedy);

struct lookup{
    chunk_t elem;
    lookup(chunk_t e):elem(e){;}
    bool operator() (centry c) const { return c.cache->fake_lookup(elem); }
};


void greedy::initialize(){
    strategy_layer::initialize();
    vector<string> ctype;
    ctype.push_back("modules.node.node");
    average_dist = 0;

    cTopology topo;
    topo.extractByNedTypeName(ctype);
    for (int i = 0;i<topo.getNumNodes();i++){
	if (i==getIndex()) continue;
	base_cache *cptr = (base_cache *)topo.getNode(i)->getModule()->getModuleByRelativePath("cache");
	cfib.push_back( centry ( cptr, FIB[i].len ) );
    }
    sort(cfib.begin(), cfib.end());
}

bool *greedy::get_decision(cMessage *in){

    bool *decision;
    if (in->getKind() == CCN_I){
	ccn_interest *interest = (ccn_interest *)in;
	decision = exploit(interest);
    }
    return decision;

}



//The nearest repository just exploit the host-centric FIB. 
bool *greedy::exploit(ccn_interest *interest){

    int repository,
	node,
	outif,
	gsize;

    gsize = getOuterInterfaces();
    bool *decision = new bool[gsize];
    std::fill(decision,decision+gsize,0);

    //find the first occurrence in the sorted vector of caches.
    vector<centry>::iterator it = std::find_if (cfib.begin(),cfib.end(),lookup(interest->getChunk()));

    vector<int> repos = interest->get_repos();
    repository = nearest(repos);

    node = -1;

    if (it!=cfib.end())
	node = it->cache->getIndex();

    bool c;
    if (node!=-1 && FIB[node].len < FIB[repository].len){
	outif = FIB[node].id;
	c=false;
    }else{
	c = true;
	outif = FIB[repository].id;
    }

//    if (interest->get_name()==92){
//	cout<<getIndex()<<"]Sending 92 toward "<<(c?repository:node)<<" at time "<<simTime()<<endl;
//    }
//
//    if (outif == interest->getArrivalGate()->getIndex())
//	cout<<"Capitoooooooooooo"<<endl;

    decision[outif] = true;
    return decision;

}

int greedy::nearest(vector<int>& repositories){
    int  min_len = 10000;
    vector<int> targets;

    for (vector<int>::iterator i = repositories.begin(); i!=repositories.end();i++){ //Find the shortest (the minimum)
        if (FIB[ *i ].len < min_len ){
            min_len = FIB[ *i ].len;
	    targets.clear();
            targets.push_back(*i);
        }else if (FIB[*i].len == min_len)
	    targets.push_back(*i);

    }
    return targets[intrand(targets.size())];
}

void greedy::finish(){
    ;
    //string id = "nodegetIndex()+"]";
}

