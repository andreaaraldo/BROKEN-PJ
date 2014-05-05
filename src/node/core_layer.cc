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

//<aa>
#include "error_handling.h"
//</aa>

Register_Class(core_layer);
int core_layer::repo_interest = 0;


void  core_layer::initialize(){
    RTT = par("RTT");
	//<aa>
	interest_aggregation = par("interest_aggregation");
	transparent_to_hops = par("transparent_to_hops");
	//</aa>
    //repo_load = 0;  //<aa> Disabled this. The reset is inside clear_stat() </aa>
    nodes = getAncestorPar("n"); //Number of nodes
    my_btw = getAncestorPar("betweenness");
    int num_repos = getAncestorPar("num_repos");

    int i = 0;
    my_bitmask = 0;
    for (i = 0; i < num_repos; i++)
	if (content_distribution::repositories[i] == getIndex())
	    break;
    my_bitmask = (1<<i);//recall that the width of the repository bitset is only num_repos

    //Getting the content store
    ContentStore = (base_cache *) gate("cache_port$o")->getNextGate()->getOwner();
    strategy = (strategy_layer *) gate("strategy_port$o")->getNextGate()->getOwner();

    //Statistics
    //interests = 0; //<aa> Disabled this. The reset is inside clear_stat() </aa>
    //data = 0; //<aa> Disabled this. The reset is inside clear_stat() </aa>

	//<aa>
	clear_stat();

	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
	#endif

	//</aa>

}



/*
 * Core layer core function. Here the incoming packet is classified,
 * determining if it is an interest or a data packet (the corresponding
 * counters are increased). The two auxiliar functions handle_interest() and
 * handle_data() have the task of dealing with interest and data processing.
 */
void core_layer::handleMessage(cMessage *in){
	//<aa>
	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
	char* last_received;
	
	#endif
	//</aa>


    ccn_data *data_msg;
    ccn_interest *int_msg;


    int type = in->getKind();
    switch(type){
    //On receiving interest
    case CCN_I:	
		interests++;

		int_msg = (ccn_interest *) in;

		//<aa>
		if (!transparent_to_hops)
		//</aa>
			int_msg->setHops(int_msg -> getHops() + 1);

		if (int_msg->getHops() == int_msg->getTTL())
		{
	    	//<aa>
	    	#ifdef SEVERE_DEBUG
	    	discarded_interests++;
	    	check_if_correct(__LINE__);
	    	#endif
	    	//</aa>
	    	break;
		}
		int_msg->setCapacity (int_msg->getCapacity() + ContentStore->get_size());
		handle_interest (int_msg);
		break;

    //On receiving data
    case CCN_D:
		data++;

		data_msg = (ccn_data* ) in; //One hop more from the last caching node (useful for distance policy)

		//<aa>
		if (!transparent_to_hops)
		//</aa>
			data_msg->setHops(data_msg -> getHops() + 1);

		handle_data(data_msg);

		break;
    }

    delete in;
    
	//<aa>
	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
	#endif
	//</aa>
}

//Per node statistics printing
void core_layer::finish(){
	//<aa>
	#ifdef SEVERE_DEBUG
	if (	data+repo_load != \
			(int) (ContentStore->get_decision_yes() + ContentStore->get_decision_no() ) 
	){
			std::stringstream msg; 
			msg<<"node["<<getIndex()<<"]: "<<
				"decision_yes=="<<ContentStore->get_decision_yes()<<
				"; decision_no=="<<ContentStore->get_decision_no()<<
				"; repo_load=="<<repo_load<<
				"; data="<<data<<
				". The sum of decision_yes+decision_no MUST be equal to data+repo_load";
		    severe_error(__FILE__, __LINE__, msg.str().c_str() );
	}
	#endif
	//</aa>

    char name [30];
    //Total interests
    sprintf ( name, "interests[%d]", getIndex());
    recordScalar (name, interests);

    if (repo_load != 0){
		sprintf ( name, "repo_load[%d]", getIndex());
		recordScalar(name,repo_load);
    }

    //Total data
    sprintf ( name, "data[%d]", getIndex());
    recordScalar (name, data);

    if (repo_interest != 0){
	sprintf ( name, "repo_int[%d]", getIndex());
	recordScalar(name, repo_interest);
	repo_interest = 0;
    }


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

        data_msg->setHops(0);
        data_msg->setBtw(int_btw); //Copy the highest betweenness
        data_msg->setTarget(getIndex());
        data_msg->setFound(true);

        data_msg->setCapacity(int_msg->getCapacity());
        data_msg->setTSI(int_msg->getHops());
        data_msg->setTSB(1);

        send(data_msg,"face$o", int_msg->getArrivalGate()->getIndex());
        
        //<aa>
        #ifdef SEVERE_DEBUG
        interests_satisfied_by_cache++;
		check_if_correct(__LINE__);
        #endif
        //</aa>

    } else if ( my_bitmask & __repo(int_msg->get_name() ) ){
	//
	//b) Look locally (only if you own a repository)
	// we are mimicking a message sent to the repository
	//
        ccn_data* data_msg = compose_data(chunk);
		repo_interest++;
		repo_load++;

        data_msg->setHops(1);
        data_msg->setTarget(getIndex());
		data_msg->setBtw(std::max(my_btw,int_btw));

		data_msg->setCapacity(int_msg->getCapacity());
		data_msg->setTSI(int_msg->getHops() + 1);
		data_msg->setTSB(1);
		data_msg->setFound(true);

        ContentStore->store(data_msg);

        send(data_msg,"face$o",int_msg->getArrivalGate()->getIndex());

		//<aa>
		#ifdef SEVERE_DEBUG
		check_if_correct(__LINE__);
		#endif
		//</aa>
   } else {
        //
        //c) Put the interface within the PIT (and follow your FIB)
        //
        
   		//<aa>
		#ifdef SEVERE_DEBUG
		unsatisfied_interests++;
		check_if_correct(__LINE__);
		#endif
		//</aa>


        unordered_map < chunk_t , pit_entry >::iterator pitIt = PIT.find(chunk);


        if (	pitIt==PIT.end() || //<aa> there is no such an entry in the PIT 
								// thus I have to forward the interest</aa>
			(pitIt != PIT.end() && int_msg->getNfound()) ||
		    simTime() - PIT[chunk].time > 2*RTT ||			
			/*//<aa>*/! interest_aggregation //</aa>
        ){
	    	bool * decision = strategy->get_decision(int_msg);
	    	handle_decision(decision,int_msg);
	    	delete [] decision;//free memory for the decision array

	    	if (pitIt!=PIT.end())
				PIT.erase(chunk);
			//<aa>Last time this entry has been updated is now</aa>
	    	PIT[chunk].time = simTime(); 
		}

		__sface(PIT[chunk].interfaces, int_msg->getArrivalGate()->getIndex());
		
		//<aa>
		#ifdef SEVERE_DEBUG
		check_if_correct(__LINE__);
		#endif
		//</aa>


    }
    
    //<aa>
    #ifdef SEVERE_DEBUG
    check_if_correct(__LINE__);
    #endif
    //</aa>
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
	if (decision[i] == true && !__check_client(i)
		&& interest->getArrivalGate()->getIndex() != i)
	    sendDelayed(interest->dup(),interest->getDelay(),"face$o",i);
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
    repo_interest = 0;
    interests = 0;
    data = 0;
    
    //<aa>
    repo_interest = 0;
    repo_load = 0;
    
   	#ifdef SEVERE_DEBUG
	discarded_interests = 0;
	unsatisfied_interests = 0;
	interests_satisfied_by_cache = 0;
	#endif
    //</aa>
}

//<aa>
#ifdef SEVERE_DEBUG
void core_layer::check_if_correct(int line)
{
	if (repo_load != interests - discarded_interests - unsatisfied_interests
		-interests_satisfied_by_cache)
	{
			std::stringstream msg; 
			msg<<"node["<<getIndex()<<"]: "<<
				"repo_load="<<repo_load<<"; interests="<<interests<<
				"; discarded_interests="<<discarded_interests<<
				"; unsatisfied_interests="<<unsatisfied_interests<<
				"; interests_satisfied_by_cache="<<interests_satisfied_by_cache;
		    severe_error(__FILE__, line, msg.str().c_str() );
	}
}
#endif
//</aa>
