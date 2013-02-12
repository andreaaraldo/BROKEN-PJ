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
#include "random_repository.h"

Register_Class(random_repository);

void random_repository::initialize(){

    strategy_layer::initialize();
    
}

void random_repository::finish(){
    ;
}

bool *random_repository::get_decision(cMessage *in){//check this function

    if (in->getKind() == CCN_I){
	ccn_interest *interest = (ccn_interest *)in;
	bool *decision = exploit(interest);
	return decision;
    }

}



bool *random_repository::exploit(ccn_interest *interest){

    int repository,
	outif,
	gsize;

    gsize = getOuterInterfaces();
    repository = random_rep (interest->get_repos());
    outif = FIB[repository].id;

    bool *decision = new bool[gsize];
    for (int i = 0; i < gsize ; i++)
	decision[i]=false;

    decision[outif]=true;

    return decision;

}


uint32_t random_repository::random_rep(vector<int> repositories){
    return repositories[intrand(repositories.size())];
}
