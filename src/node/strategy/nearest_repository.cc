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
#include "nearest_repository.h"
#include "packets/ccn_interest.h"

Register_Class(nearest_repository);

void nearest_repository::initialize(){

    strategy_layer::initialize();
    
}


void nearest_repository::finish(){
    ;
}

bool *nearest_repository::get_decision(cMessage *in){

    if (in->getKind() == CCN_I){
	ccn_interest *interest = (ccn_interest *)in;
	bool *decision = exploit(interest);
	return decision;
    }

}



//The nearest repository just exploit the host-centric FIB. 
bool *nearest_repository::exploit(ccn_interest *interest){

    int repository,
	outif,
	gsize;

    gsize = getOuterInterfaces();
    repository = nearest_rep(interest->get_repos());
    //repository = n-1;
    outif = FIB[repository].id;

    bool *decision = new bool[gsize];
    for (int i =0;i<gsize;i++)
	decision[i]=false;
    decision[outif]=true;

    return decision;

}


//Return the nearest repostiory for a a given content
uint32_t nearest_repository::nearest_rep(vector<int> repositories){
    uint32_t path_len = 10000;
    uint32_t target;

    for (vector<int>::iterator i = repositories.begin(); i!=repositories.end();i++){ //Find the shortest (the minimum)
	if (FIB[ *i ].len < path_len){
	    path_len = FIB[ *i ].len;
	    target = *i;
	}
    }

    return target;
}
