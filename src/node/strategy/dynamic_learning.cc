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
#include "dynamic_learning.h"
Register_Class(dynamic_learning);


void dynamic_learning::initialize(){

    strategy_layer::initialize(); 
}

void dynamic_learning::finish(){
    ;
}

bool *dynamic_learning::get_decision(cMessage *in){//check this function
    int arrival_gate = 0,
	target;
    bool *decision;
    ccn_interest *interest;

    if (in->getKind() == CCN_I){
	interest = (ccn_interest *)in; //safely cast
	target = interest->getTarget();

	if (target != -1 && target != getIndex() ){
	    //exploit the target
	    decision = exploit(interest);
	}else{
	    //explore the network
	    decision = explore(interest);
	}
    }
    return decision;

}


bool *dynamic_learning::explore(ccn_interest *interest){
    int arrival_gate,
	gsize;
    bool *decision;

    gsize = getOuterInterfaces();
    arrival_gate = interest->getArrivalGate()->getIndex();
    decision = new bool[gsize];

    for (int i =0; i < gsize; i++)
	if (i != arrival_gate && !check_client(i))
	    decision[i] = true;
	else
	    decision[i] = false;
    return decision;
}




bool *dynamic_learning::exploit(ccn_interest *interest){

    bool *decision;
    int repository,
	outif,
	gsize,
	target;

    gsize = getOuterInterfaces();
    target = interest->getTarget();

    outif = FIB[target].id;

    decision = new bool[gsize];
    for (int i =0;i<gsize;i++)
	decision[i]=false;
    decision[outif]=true;

    return decision;

}



