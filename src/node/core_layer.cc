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
#include "ccn_data.h"

Register_Class(core_layer);


void  core_layer::initialize(){
    nodes = getAncestorPar("n"); //Number of nodes
    
    cTopology topo;
    vector<string> types;
    types.push_back("modules.node.node");
    topo.extractByNedTypeName( types );
    cTopology::Node *current = topo.getNode( getIndex() );

    //-------------------
    //Central calculations
    topo.betweenness_centrality();
    this->btw = current->get_betweenness();
    //-------------------

    RTT = par("RTT");


    //Getting the content store
    ContentStore = (base_cache *) gate("cache_port$o")->getNextGate()->getOwner();
    strategy = (strategy_layer *)gate("strategy_port$o")->getNextGate()->getOwner();

    //Statistics
    interests = 0;
    data = 0;


}

double core_layer::get_betweenness(){
    return btw;
}


//Core function of a core_layer
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



//Handling incoming interests:
//if an interest for a data file arrives: 
//
//
//     a) Check in your Content Store
//     
//     b) Check if you are the source for that data. 
//     
//     c) Put the interface within the PIT.
//
//
void core_layer::handle_interest(ccn_interest *int_msg){
    uint64_t chunk = int_msg->getChunk();
    double h_btw = int_msg->getBtw();

    if (ContentStore->lookup(chunk)){
        //
        //a) Check in your Content Store
        //
        ccn_data* data_msg = compose_data(chunk);
        data_msg->setTarget(getIndex());
        data_msg->setHops(0);
        data_msg->setTimestamp(simTime());
        data_msg->setBtw(h_btw); //Copy the highest betweenness
        send(data_msg,"face$o", int_msg->getArrivalGate()->getIndex());

    } else if ( check_ownership( int_msg->get_repos() ) ){
    //} else if ( getIndex() == nodes-1 ){
	//
	//b) Look locally (only if you own a repository)
	// we are mimicking a message sent to the repository
	//
        ccn_data* data_msg = compose_data(chunk);
        data_msg->setTarget(getIndex());
        data_msg->setHops(1);
        data_msg->setTimestamp(simTime());

	if ( btw > h_btw )
	    data_msg ->setBtw(btw);
	else 
	    data_msg->setBtw(h_btw);

        ContentStore->received_data(data_msg);
        send(data_msg,"face$o",int_msg->getArrivalGate()->getIndex());


    } else {
	//
        //c) Put the interface within the PIT (and follow your FIB)
	//

	if (int_msg->getTarget() == getIndex())//failure
	    int_msg->setTarget(-1);

	unordered_map < uint64_t, pit_entry >::iterator pitIt = PIT.find(chunk);
	if (pitIt==PIT.end()){
	    int_msg->setIif(int_msg->getArrivalGate()->getIndex());
	    bool * decision = strategy->get_decision(int_msg);
	    handle_decision(decision,int_msg);
	    delete [] decision;//free memory for the decision array
	}


	__sface(PIT[chunk].interfaces, int_msg->getArrivalGate()->getIndex());

	if (PIT.size()>max_pit)
	
	    max_pit = PIT.size();

    }
}



//Manage incoming data
void core_layer::handle_data(ccn_data *data_msg){


    int i = 0;
    interface_t interfaces = 0;
    uint64_t chunk = data_msg -> getChunk(); //Get information about the file

    unordered_map < uint64_t, pit_entry >::iterator pitIt = PIT.find(chunk);

    //If someone had previously requested the data 
    if ( pitIt != PIT.end() ){

	ContentStore->received_data(data_msg);
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
    if (btw > interest->getBtw())
	interest->setBtw(btw);

    for (int i = 0; i < getOuterInterfaces(); i++)
	if (decision[i] == true && !check_client(i))
	    send(interest->dup(),"face$o",i);
}


bool core_layer::check_ownership(vector<int> repositories){
    bool check = false;
    if (find (repositories.begin(),repositories.end(),getIndex()) != repositories.end())
	check = true;
    return check;
}



//Compose a data response packet
ccn_data* core_layer::compose_data(uint64_t response_data){
    ccn_data* data = new ccn_data("data",CCN_D);
    data -> setChunk (response_data);
    data -> setHops(0);
    return data;
}

//Clear local statistics
void core_layer::clear_stat(){
    interests = 0;
    data = 0;
    return;
}
