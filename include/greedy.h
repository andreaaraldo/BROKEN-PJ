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
#include "ccn_interest.h"

Register_Class(greedy);


void greedy::initialize(){
    strategy_layer::initialize();
    cTopology topo;
    topo.extractByNedTypes("modules.node.cache");
    for (int i = 0;i<topo.getNodes();i++){
	if (i==getIndex()) continue;
	caches.push_back( (base_cache)topo.getNode(i)->getModule());
    }
    sort(caches.begin(), cache.end(),cmp);
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
	outif,
	gsize;

    gsize = getOuterInterfaces();

    vector<int> repos = interest->get_repos();
    repository = nearest(repos);

    outif = FIB[repository].id;

    bool *decision = new bool[gsize];
    std::fill(decision,decision+gsize,0);
    decision[outif]=true;

    return decision;

}
