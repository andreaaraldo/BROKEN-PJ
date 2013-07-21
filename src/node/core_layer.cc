/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer)
 *    Raffaele Chiocchetti (developer)
 *    Dario Rossi (occasional debugger)
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
#include "core_layer.h"
#include "ccnsim.h"
#include <algorithm>

#include "content_distribution.h"
#include "strategy_layer.h"
#include "ccn_interest.h"
#include "ccn_data.h"
#include "base_cache.h"

Register_Class(core_layer);


void  core_layer::initialize(){
    nodes = getAncestorPar("n"); //Number of nodes
    my_btw = getAncestorPar("betweenness");
    int num_repos = getAncestorPar("num_repos");

    int i = 0;
    my_bitmask = 0;
    for (i = 0; i < num_repos; i++)
	if (content_distribution::repositories[i] == getIndex())
	    break;
    my_bitmask = (1<<i);//recall that the width of the repository bitset is only num_repos
    //cout<<getIndex()<<"]"<<my_bitmask<<endl;
    //Getting the content store
    ContentStore = (base_cache *) gate("cache_port$o")->getNextGate()->getOwner();
    strategy = (strategy_layer *) gate("strategy_port$o")->getNextGate()->getOwner();

    //Statistics
    interests = 0;
    data = 0;


}



/*
 * Core layer core function. Here the incoming packet is classified,
 * determining if it is an interest or a data packet (the corresponding
 * counters are increased). The two auxiliar functions handle_interest() and
 * handle_data() have the task of dealing with interest and data processing.
 */
void core_layer::handleMessage(cMessage *in){

    ccn_data *data_msg;
    ccn_interest *int_msg;


    int type = in->getKind();
    switch(type){
    //On receiving interest
    case CCN_I:	
	interests++;

	int_msg = (ccn_interest *) in;
	int_msg->setHops(int_msg -> getHops() + 1);

	if (int_msg->getHops() == int_msg->getTTL()){
	    break;
	}

	handle_interest (int_msg);

	break;
    //On receiving data
    case CCN_D:
	data++;

	data_msg = (ccn_data* ) in; //One hop more from the last caching node (useful for distance policy)
	data_msg->setHops(data_msg -> getHops() + 1);

	handle_data(data_msg);

	break;
    }

    delete in;
}

//Per node statistics printing
void core_layer::finish(){
    char name [30];
    //Total interests
    sprintf ( name, "interests[%d]", getIndex());
    recordScalar (name, interests);

    //Total data
    sprintf ( name, "data[%d]", getIndex());
    recordScalar (name, data);

}



/* Handling incoming interests:
*  if an interest for a given content comes up: 
*     a) Check in your Content Store
*     b) Check if you are the source for that data. 
*     c) Put the interface within the PIT.
*/
void core_layer::handle_interest(ccn_interest *int_msg){
    chunk_t chunk = int_msg->getChunk();
    double int_btw = int_msg->getBtw();

    if (ContentStore->lookup(chunk)){
        //
        //a) Check in your Content Store
        //
        ccn_data* data_msg = compose_data(chunk);
        data_msg->setTarget(getIndex());
        data_msg->setBtw(int_btw); //Copy the highest betweenness
        send(data_msg,"face$o", int_msg->getArrivalGate()->getIndex());

    } else if ( my_bitmask & __repo(int_msg->get_name() ) ){
	//
	//b) Look locally (only if you own a repository)
	// we are mimicking a message sent to the repository
	//
        ccn_data* data_msg = compose_data(chunk);
        data_msg->setTarget(getIndex());
        data_msg->setHops(1);
	data_msg->setBtw(std::max(my_btw,int_btw));

        ContentStore->store(data_msg);
        send(data_msg,"face$o",int_msg->getArrivalGate()->getIndex());


    } else {
	//
        //c) Put the interface within the PIT (and follow your FIB)
	//

	unordered_map < chunk_t , pit_entry >::iterator pitIt = PIT.find(chunk);

	if (pitIt==PIT.end() || 
		pitIt->second.timer > TTL //Timer is expired
		
		){
	    bool * decision = strategy->get_decision(int_msg);
	    handle_decision(decision,int_msg);
	    delete [] decision;//free memory for the decision array
	}


	__sface(PIT[chunk].interfaces, int_msg->getArrivalGate()->getIndex());


    }
}



/*
 * Handle incoming data packets. First check within the PIT if there are
 * interfaces interested for the given content, then (try to) store the object
 * within your content store. Finally propagate the interests towards all the
 * interested interfaces.
 */
void core_layer::handle_data(ccn_data *data_msg){


    int i = 0;
    interface_t interfaces = 0;
    chunk_t chunk = data_msg -> getChunk(); //Get information about the file

    unordered_map < chunk_t , pit_entry >::iterator pitIt = PIT.find(chunk);

    //If someone had previously requested the data 
    if ( pitIt != PIT.end() ){

	ContentStore->store(data_msg);
	interfaces = (pitIt->second).interfaces;//get interface list
	i = 0;
	while (interfaces){
	    if ( interfaces & 1 )
		send(data_msg->dup(), "face$o", i ); //follow bread crumbs back
	    i++;
	    interfaces >>= 1;
	}
    }
    PIT.erase(chunk); //erase pending interests for that data file
}


void core_layer::handle_decision(bool* decision,ccn_interest *interest){
    if (my_btw > interest->getBtw())
	interest->setBtw(my_btw);

    for (int i = 0; i < __get_outer_interfaces(); i++)
	if (decision[i] == true && !__check_client(i))
	    send(interest->dup(),"face$o",i);
}


bool core_layer::check_ownership(vector<int> repositories){
    bool check = false;
    if (find (repositories.begin(),repositories.end(),getIndex()) != repositories.end())
	check = true;
    return check;
}



/*
 * Compose a data response packet
 */
ccn_data* core_layer::compose_data(uint64_t response_data){
    ccn_data* data = new ccn_data("data",CCN_D);
    data -> setChunk (response_data);
    data -> setHops(0);
    data->setTimestamp(simTime());
    return data;
}

/*
 * Clear local statistics
 */
void core_layer::clear_stat(){
    interests = 0;
    data = 0;
}
