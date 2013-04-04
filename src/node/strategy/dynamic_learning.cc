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
#include "dynamic_learning.h"
#include "packets/ccn_interest.h"
Register_Class(dynamic_learning);


bool *dynamic_learning::get_decision(cMessage *in){//check this function
    int target;
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

    memset(decision,0,gsize);
    decision[arrival_gate] = true;
    return decision;
}




bool *dynamic_learning::exploit(ccn_interest *interest){

    bool *decision;
    int outif,
	gsize,
	target;

    gsize = getOuterInterfaces();
    target = interest->getTarget();

    outif = FIB[target].id;

    decision = new bool[gsize];
    memset(decision,0,gsize);
    decision[outif]=true;

    return decision;

}



