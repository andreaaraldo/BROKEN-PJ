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
#include "ccn_interest.h"
Register_Class(dynamic_learning);


bool *dynamic_learning::get_decision(cMessage *in){//check this function
    int target;
    bool *decision;
    ccn_interest *interest;


    if (in->getKind() == CCN_I){
	interest = (ccn_interest *)in; //safely cast
	target = interest->getTarget();

	if (target != -1  ){
	//if (target != -1 && target != getIndex() ){
	    //exploit the target
	    decision = exploit(interest);
	}else{
	    //explore the network
	    decision = explore(interest);
	}
    }
    return decision;

}


/*
 * Explore the network if the target is not yet defined. The target is the node
 * (repository or cache) which stores the nearest copy of the data.
 */
bool *dynamic_learning::explore(ccn_interest *interest){
    int arrival_gate,
	gsize;
    bool *decision;

    gsize = getOuterInterfaces();
    arrival_gate = interest->getArrivalGate()->getIndex();
    decision = new bool[gsize];

    std::fill(decision,decision+gsize,1);
    decision[arrival_gate] = false;
    return decision;
}




/*
 * Exploit the knowledge of the network acquired during the exploration phase.
 * Of course, the given target may withdraw the content. In this case is the
 * given target that explores again the network looking for content close to
 * himself.
 */
bool *dynamic_learning::exploit(ccn_interest *interest){


    bool *decision;
    int outif,
	gsize,
	target;

    gsize = getOuterInterfaces();
    target = interest->getTarget();

    if (interest->getTarget() == getIndex()){//failure
	interest->setTarget(-1);
	return explore(interest);
    }

    outif = FIB[target].id;

    decision = new bool[gsize];
    std::fill(decision,decision+gsize,0);
    decision[outif]=true;

    return decision;

}

