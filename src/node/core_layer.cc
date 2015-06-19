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
#include "two_lru_policy.h"
//<aa>
#include "error_handling.h"
#include "repository/Repository.h"
#include "PIT.h"
//</aa>

Register_Class(core_layer);


void  core_layer::initialize()
{
	//<aa>
	#ifdef SEVERE_DEBUG
		i_am_initializing = true;
		is_it_initialized = false;
		it_has_a_repo_attached = false;
	#endif
	interest_aggregation = par("interest_aggregation");
	transparent_to_hops = par("transparent_to_hops");
	// Notice that repo_price has been initialized by WeightedContentDistribution
	//</aa>
 
	double RTT = par("RTT");
    nodes = getAncestorPar("n"); //Number of nodes
    my_btw = getAncestorPar("betweenness");


    //Getting the content store
    ContentStore = (base_cache *) gate("cache_port$o")->getNextGate()->getOwner();
    strategy = (strategy_layer *) gate("strategy_port$o")->getNextGate()->getOwner();

	//<aa>
	initialize_iface_stats();
	repository = create_repository();

	#ifdef SEVERE_DEBUG
		it_has_a_repo_attached = true;
	#endif


	clear_stat();

	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
	is_it_initialized = true;

	if (gateSize("face$o") > (int) sizeof(interface_t)*8 )
	{
		std::stringstream msg;
		msg<<"Node "<<getIndex()<<" has "<<gateSize("face$o")<<" output ports. But the maximum "
			<<"number of interfaces manageable by ccnsim is "<<sizeof(interface_t)*8 <<
			" beacause the type of "
			<<"interface_t is of size "<<sizeof(interface_t)<<" bytes. You can change the definition of "
			<<"interface_t (in ccnsim.h) to solve this issue and recompile";
		severe_error(__FILE__, __LINE__, msg.str().c_str() );
	}

	i_am_initializing = false;
	#endif

	pit = new PIT(RTT);
	//</aa>

}

//<aa>
void core_layer::initialize_iface_stats()
{
	iface_stats = (iface_stats_t*) calloc(gateSize("face$o"), sizeof(iface_stats_t) );
}


Repository* core_layer::create_repository()
{
    int num_repos = getAncestorPar("num_repos");
	Repository* repository_ = NULL;
    int repo_index = 0;
    for (repo_index = 0; repo_index < num_repos; repo_index++)
	{
		if (content_distribution::repositories[repo_index] == getIndex() )
		{
			double price = content_distribution::repo_prices[repo_index]; 
			repository_ = new Repository(getIndex(), repo_index, price);
			break;
		} 
	}
	return repository_;
}
//</aa>

/*
 * Core layer core function. Here the incoming packet is classified,
 * determining if it is an interest or a data packet (the corresponding
 * counters are increased). The two auxiliar functions handle_interest() and
 * handle_data() have the task of dealing with interest and data processing.
 */
void core_layer::handleMessage(cMessage *in)
{
	//<aa>
	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
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
		int_msg->setCapacity (int_msg->getCapacity() + ContentStore->get_slots());
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

//	Print node statistics
void core_layer::finish()
{
	//<aa>
	#ifdef SEVERE_DEBUG
	check_if_correct(__LINE__);
	#endif
	//</aa>

    char name [30];

    //Total interests
	// <aa> Parts of these interests will be satisfied by the local cache; the remaining part will be sent to the local repo (if present) and partly sarisfied there. For the remaining part, a FIB entry will be searched to forward the intereset. If no FIB entry is found, the interest will be discarded </aa>
    sprintf ( name, "interests[%d]", getIndex());
    recordScalar (name, interests);

	if (repository != NULL)	
		repository->finish(this);

    //Total data
    sprintf ( name, "data[%d]", getIndex());
    recordScalar (name, data);

	//<aa>
	const char* gatename = "face$o";
	for (int j=0; j<gateSize(gatename); j++)
	{
		const char* this_gate = gate(gatename, j)->getFullName();
		cGate* border_gate_of_this_node = gate(gatename, j)->getNextGate();
		const char* other_node = border_gate_of_this_node->getNextGate()->getOwnerModule()->getFullName();
		sprintf ( name, "megabytes_sent[%s->%s]", this_gate, other_node);
		recordScalar(name, iface_stats[j].megabytes_sent );
	}
	//</aa>
}




/* Handling incoming interests:
*  if an interest for a given content comes up: 
*     a) Check in your Content Store
*     b) Check if you are the source for that data. 
*     c) Put the interface within the PIT.
*/
void core_layer::handle_interest(ccn_interest *int_msg)
{
	//<aa>
	#ifdef SEVERE_DEBUG
		client* cli = __get_attached_client( int_msg->getArrivalGate()->getIndex() );
		if (cli && !cli->is_active() ) {
			std::stringstream msg; 
			msg<<"I am node "<< getIndex()<<" and I received an interest from interface "<<
				int_msg->getArrivalGate()->getIndex()<<". This is an error since there is "<<
				"a deactivated client attached there";
			debug_message(__FILE__, __LINE__, msg.str().c_str() );
		}
	#endif
	//</aa>

 
   chunk_t chunk = int_msg->getChunk();

   double int_btw = int_msg->getBtw();
   bool cacheable = true;  // This value indicates whether the retrieved content will be cached.
    
    // Check if the meta-caching is 2-LRU. In this case, we need to lookup for the content ID inside the Name Cache.
    string decision_policy = ContentStore->par("DS");

    if (decision_policy.compare("two_lru")==0)
    {
    	Two_Lru* tLruPointer = (Two_Lru *) (ContentStore->get_decisor());
    	if (!(tLruPointer->name_to_cache(chunk)))	// The ID is not present inside the Name Cache, so the
    												// cacheable flag inside the PIT will be set to '0'.
    			cacheable = false;
    }


	


    unsigned short selected_data_representation = 0;
  if (ContentStore->lookup(chunk))
  {
       //
       //a) Check in your Content Store
       //
        ccn_data* data_msg = compose_data(chunk,1);

        data_msg->setHops(0);
        data_msg->setBtw(int_btw); //Copy the highest betweenness
        data_msg->setTarget(getIndex());
        data_msg->setFound(true);

        data_msg->setCapacity(int_msg->getCapacity());
        data_msg->setTSI(int_msg->getHops());
        data_msg->setTSB(1);

		//<aa> I transformed send in send_data</aa>
        send_data(data_msg,"face$o", int_msg->getArrivalGate()->getIndex(), __LINE__); 
        
        //<aa>
        #ifdef SEVERE_DEBUG
        interests_satisfied_by_cache++;
		check_if_correct(__LINE__);
        #endif
        //</aa>

    } else if ( repository!=NULL && (selected_data_representation = repository->handle_interest(int_msg ) ) )
	{		
			//
			//b) Look locally (only if you own a repository)
			// we are mimicking a message sent to the repository
			//
		    ccn_data* data_msg = compose_data(chunk, selected_data_representation );
	
			//<aa>
			data_msg->setPrice(repository->get_price() ); 	// I fix in the data msg the cost of the object
											// that is the price of the repository
			//</aa>

		    data_msg->setHops(1);
		    data_msg->setTarget(getIndex());
			data_msg->setBtw(std::max(my_btw,int_btw));

			data_msg->setCapacity(int_msg->getCapacity());
			data_msg->setTSI(int_msg->getHops() + 1);
			data_msg->setTSB(1);
			data_msg->setFound(true);

		    ContentStore->store(data_msg);

			//<aa> I transformed send in send_data</aa>
			send_data(data_msg,"face$o",int_msg->getArrivalGate()->getIndex(),__LINE__);

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

		//<aa>
		bool i_will_forward_interest = false;
		//</aa>

		bool previously_found_in_pit = pit->handle_interest(int_msg, cacheable);
        if ( previously_found_in_pit==false )
		{
			//<aa> Replaces the lines
			//		bool * decision = strategy->get_decision(int_msg);
			//		handle_decision(decision,int_msg);
			// 		delete [] decision;//free memory for the decision array
			i_will_forward_interest = true;
			//</aa>
		}

		//<aa>
		if (int_msg->getTarget() == getIndex() )
		{	// I am the target of this interest but I have no more the object
			// Therefore, this interest cannot be aggregated with the others
			int_msg->setAggregate(false);
		}

		if ( !interest_aggregation || int_msg->getAggregate()==false )
			i_will_forward_interest = true;

		if (i_will_forward_interest)
		{  	bool * decision = strategy->get_decision(int_msg);
	    	handle_decision(decision,int_msg);
	    	delete [] decision;//free memory for the decision array
		}
		#ifdef SEVERE_DEBUG
			check_if_correct(__LINE__);

			client*  c = __get_attached_client( int_msg->getArrivalGate()->getIndex() );
			if (c && !c->is_active() ){
				std::stringstream ermsg; 
				ermsg<<"Trying to add to the PIT an interface where a deactivated client is attached";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
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
void core_layer::handle_data(ccn_data *data_msg)
{

    int i = 0;
    interface_t interfaces = 0;

	//<aa>
	#ifdef SEVERE_DEBUG
		int copies_sent = 0;
	#endif
	//</aa>

	pit_entry pentry = pit->handle_data(data_msg);

    if ( pentry.interfaces!=0 )
	{

		// A pit entry for this chunk was found
    	if (pentry.cacheable.test(0))  // Cache the content only if the cacheable bit is set.
    		ContentStore->store(data_msg);
		else
			ContentStore->after_discarding_data();

    	interfaces = pentry.interfaces;	// Get incoming interfaces.
		i = 0;
		while (interfaces)
		{
			if ( interfaces & 1 )
			{
				//<aa> I transformed send in send_data</aa>
				send_data(data_msg->dup(), "face$o", i,__LINE__ ); //follow bread crumbs back

				//<aa>
				#ifdef SEVERE_DEBUG
					copies_sent++;
				#endif
				//</aa>
			}
			i++;
			interfaces >>= 1;
		}
    } 
	//<aa> 
	// Otherwise the data are unrequested
	#ifdef SEVERE_DEBUG
		else unsolicited_data++;
		check_if_correct(__LINE__);
	#endif
	//</aa>
}


void core_layer::handle_decision(bool* decision,ccn_interest *interest){
	//<aa>
	#ifdef SEVERE_DEBUG
	bool interest_has_been_forwarded = false;
	#endif
	//</aa>

    if (my_btw > interest->getBtw())
		interest->setBtw(my_btw);

    for (int i = 0; i < __get_outer_interfaces(); i++)
	{
		//<aa>
		#ifdef SEVERE_DEBUG
			if (decision[i] == true && __check_client(i) )
			{
				std::stringstream msg; 
				msg<<"I am node "<< getIndex()<<" and the interface supposed to give"<<
					" access to chunk "<< interest->getChunk() <<" is "<<i
					<<". This is impossible "<<
					" since that interface is to reach a client and you cannot access"
					<< " a content from a client ";
				severe_error(__FILE__, __LINE__, msg.str().c_str() );
			}
		#endif
		//</aa>

		if (decision[i] == true && !__check_client(i)
			//&& interest->getArrivalGate()->getIndex() != i
		){
			sendDelayed(interest->dup(),interest->getDelay(),"face$o",i);
			#ifdef SEVERE_DEBUG
			interest_has_been_forwarded = true;
			#endif
		}
	}
	//<aa>
	#ifdef SEVERE_DEBUG
		if (! interest_has_been_forwarded)
		{
			int affirmative_decision_from_arrival_gate = 0;
			int affirmative_decision_from_client = 0;
			int last_affermative_decision = -1;

			for (int i = 0; i < __get_outer_interfaces(); i++)
			{
				if (decision[i] == true)
				{
					if ( __check_client(i) ){
						affirmative_decision_from_client++;
						last_affermative_decision = i;
					}
					if ( interest->getArrivalGate()->getIndex() == i ){
						affirmative_decision_from_arrival_gate++;
						last_affermative_decision = i;
					}
				}
			}
			std::stringstream msg; 
			msg<<"I am node "<< getIndex()<<" and interest for chunk "<<
				interest->getChunk()<<" has not been forwarded. "<<
				". One of the possible repositories of this chunk is "<< 
				interest->get_repos()[0] <<" and the target of the interest is "<<
				interest->getTarget() <<
				". affirmative_decision_for_client = "<<
				affirmative_decision_from_client<<
				". affirmative_decision_for_arrival_gate = "<<
				affirmative_decision_from_arrival_gate<<
				". I would have sent the interest to interface "<<
				last_affermative_decision;
			severe_error(__FILE__, __LINE__, msg.str().c_str() );
		}
	#endif
}

// Check if the local node is the owner of the requested content.
bool core_layer::check_ownership(vector<int> repositories){
    bool check = false;
    if (find (repositories.begin(),repositories.end(),getIndex()) != repositories.end())
	check = true;
    return check;
}



/*
 * 	Create a Data packet in response to the received Interest.
 */
ccn_data* core_layer::compose_data(chunk_t chunk_id, unsigned short representation)
{
	representation_mask_t representation_mask = 0x0001 << (representation-1);
	__srepresentation_mask(chunk_id, representation_mask);

    ccn_data* data = new ccn_data("data",CCN_D);
    data -> setChunk (chunk_id);
    data -> setHops(0);
    data->setTimestamp(simTime());

	#ifdef SEVERE_DEBUG
		ccn_data::check_representation_mask(chunk_id);
	#endif
    return data;
}

/*
 * Clear local statistics
 */
void core_layer::clear_stat(){
    interests = 0;
    data = 0;
    
    //<aa>
	if (repository !=NULL)
		repository->clear_stat();
    ContentStore->set_decision_yes(0);
	ContentStore->set_decision_no(0);
	
	//Reset the per-interface statistics
	memset(iface_stats, 0, sizeof(iface_stats_t)*gateSize("face$o") );
	

    
   	#ifdef SEVERE_DEBUG
	unsolicited_data = 0;
	discarded_interests = 0;
	unsatisfied_interests = 0;
	interests_satisfied_by_cache = 0;
	check_if_correct(__LINE__);
	#endif
    //</aa>
}

//<aa>
#ifdef SEVERE_DEBUG
void core_layer::check_if_correct(int line)
{
	int repo_load = get_attached_repository()==NULL? 0 : get_attached_repository()->get_repo_load();

	if ( repo_load != interests - discarded_interests - unsatisfied_interests
		-interests_satisfied_by_cache)
	{
			std::stringstream msg; 
			msg<<"node["<<getIndex()<<"]: "<<
				"repo_load="<<get_attached_repository()->get_repo_load() <<"; interests="<<interests<<
				"; discarded_interests="<<discarded_interests<<
				"; unsatisfied_interests="<<unsatisfied_interests<<
				"; interests_satisfied_by_cache="<<interests_satisfied_by_cache;
		    severe_error(__FILE__, line, msg.str().c_str() );
	}


	if (	ContentStore->get_decision_yes() + ContentStore->get_decision_no() +  
						(unsigned) unsolicited_data
						!=  (unsigned) data + repo_load
	){
					std::stringstream ermsg; 
					ermsg<<"caches["<<getIndex()<<"]->decision_yes="<<ContentStore->get_decision_yes()<<
						"; caches[i]->decision_no="<< ContentStore->get_decision_no()<<
						"; cores[i]->data="<< data<<
						"; cores[i]->repo_load="<< get_attached_repository()->get_repo_load()<<
						"; cores[i]->unsolicited_data="<< unsolicited_data<<
						". The sum of "<< "decision_yes + decision_no + unsolicited_data must be data";
					severe_error(__FILE__,line,ermsg.str().c_str() );
	}
} //end of check_if_correct(..)
#endif

const Repository* core_layer::get_attached_repository()
{
	#ifdef SEVERE_DEBUG
	if (!is_it_initialized and ! i_am_initializing)
	{
			std::stringstream msg; 
			msg<<"I am node "<< getIndex()<<". Someone called this method before I was"
				" initialized";
			severe_error(__FILE__, __LINE__, msg.str().c_str() );
	}
	#endif
	return repository;
}


int	core_layer::send_data(ccn_data* msg, const char *gatename, int gateindex, int line_of_the_call)
{
	//{CHECKS
		#ifdef SEVERE_DEBUG
		if (gateindex > gateSize("face$o")-1 )
		{
			std::stringstream msg;
			msg<<"I am node "<<getIndex() <<". Line "<<line_of_the_call<<
				" commands you to send a packet to interface "<<gateindex<<
				". But the number of ports is "<<gateSize("face$o");
			severe_error(__FILE__, __LINE__, msg.str().c_str() );
		}

		if ( gateindex > (int) sizeof(interface_t)*8-1 )
		{
			std::stringstream msg;
			msg<<"You are trying to send a packet through the interface gateindex. But the maximum interface "
				<<"number manageable by ccnsim is "<<sizeof(interface_t)*8-1 <<" beacause the type of "
				<<"interface_t is of size "<<sizeof(interface_t)<<". You can change the definition of "
				<<"interface_t (in ccnsim.h) to solve this issue and recompile";
			severe_error(__FILE__, __LINE__, msg.str().c_str() );
		}

		client* c = __get_attached_client(gateindex);
		if (c)
		{	//There is a client attached to that port
			if ( !c->is_waiting_for( msg->get_object_id() ) )
			{
				std::stringstream msg; 
				msg<<"I am node "<< getIndex()<<". I am sending a data to the attached client that is not "<<
					" waiting for it. This is not necessarily an error, as this data could have been "
					<<" requested by the client and the client could have retrieved it before and now"
					<<" it may be fine and not wanting the data anymore. If it is the case, "<<
					"ignore this message ";
				debug_message(__FILE__, __LINE__, msg.str().c_str() );
			}

			if ( !c->is_active() )
			{
				std::stringstream msg; 
				msg<<"I am node "<< getIndex()<<". I am sending a data to the attached client "<<
					", that is not active, "<<
					" through port "<<gateindex<<". This was commanded in line "<< line_of_the_call;
				severe_error(__FILE__, __LINE__, msg.str().c_str() );
			}
		}

		ccn_data::check_representation_mask(msg->getChunk() );
		#endif
	//}CHECKS


	iface_stats[gateindex].megabytes_sent += msg->getMegabyteLength();

	return send (msg, gatename, gateindex);
}
//</aa>
