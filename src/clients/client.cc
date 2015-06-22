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
#include "content_distribution.h"

#include "statistics.h"

#include "ccn_interest.h"
#include "ccn_data.h"

#include "ccnsim.h"
#include "client.h"

//<aa>
#include <iostream>
#include "error_handling.h"
//</aa>

Register_Class (client);


void client::initialize()
{
	is_it_proactive_component_ = 
		getModuleType() == cModuleType::find("modules.clients.ProactiveComponent") ?
		true: false;

    int num_clients = getAncestorPar("num_clients");
    active = false;

    if (find(	content_distribution::clients , 
				content_distribution::clients + num_clients ,getNodeIndex()
			) != content_distribution::clients + num_clients
		|| is_it_proactive_component_	// If I am a proactive_component,
										// I always activate
	){

		active = true;

		//Parameters initialization
		check_time      = par("check_time");
		lambda          = par ("lambda");
		RTT             = par("RTT");

		//Allocating file statistics
		client_stats = new client_stat_entry[__file_bulk+1];

		//Initialize average stats
		avg_distance = 0;
		avg_time = 0;
		tot_downloads = 0;
		tot_chunks = 0;

		//<aa>
		#ifdef SEVERE_DEBUG
			interests_sent = 0;
		#endif
		//</aa>

		if (lambda > 0)
		{
			arrival = new cMessage("arrival", ARRIVAL );
			timer = new cMessage("timer", TIMER);
			scheduleAt( simTime() + exponential(1./lambda), arrival);
			scheduleAt( simTime() + check_time, timer  );
		}
    }
}


void client::handleMessage(cMessage *in)
{
    if (in->isSelfMessage())
	{
		handle_timers(in);
		return;
    }

    switch (in->getKind() )
	{
		case CCN_D:
		{
			//<aa>
			#ifdef SEVERE_DEBUG
			if (!active){
				std::stringstream ermsg; 
				ermsg<<"I am the client attached to node "<<getNodeIndex()<<
					". I received a data but I shoud not as I am not active";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			#endif
			//</aa>

			ccn_data *data_message = (ccn_data *) in;
			handle_incoming_chunk (data_message);
			delete  data_message;
			break; //<aa> I added this line</aa>
		}
		//<aa>
		#ifdef SEVERE_DEBUG
		default:
			std::stringstream ermsg; 
			ermsg<<"A client can only receive data, while this is a message"<<
				" of a kind "<<in->getKind();
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		#endif
		//</aa>
    }

	//<aa>
	#ifdef SEVERE_DEBUG
		if (client_stats == 0)
		{
			std::stringstream ermsg; 
			ermsg<<"The pointer client_stats points to 0. This is an error."<<
				" Is this node active? "<<active;
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif
	//</aa>
}

int client::getNodeIndex(){
    return gate("client_port$o")->getNextGate()->getOwnerModule()->getIndex();

}

//Output average local statistics
void client::finish()
{
	if ( is_it_proactive_component_ && 
		avg_distance + tot_downloads + avg_time + interests_sent == 0
	){ 
		// I am not a real client but a proactive component of node
	  	// All my statistics are 0; this means I have never been called during
		// simulation. Therefore, it is not worth printing statistics
		return;
	}
	
	const char* type = is_it_proactive_component_? "proactive_component":"real";

    if (active)
	{
		char name [30];
		sprintf ( name, "hdistance[%d].%s", getNodeIndex(), type);
		recordScalar (name, avg_distance);

		sprintf ( name, "downloads[%d].%s",getNodeIndex(), type);
		recordScalar (name, tot_downloads );

		sprintf ( name, "avg_time[%d].%s",getNodeIndex(), type);
		recordScalar (name, avg_time);

		//<aa>
		#ifdef SEVERE_DEBUG
			sprintf ( name, "interests_sent[%d].%s",getNodeIndex(), type);
			recordScalar (name, interests_sent );

			if (interests_sent != tot_downloads){
				std::stringstream ermsg; 
				ermsg<<"interests_sent="<<interests_sent<<"; tot_downloads="<<tot_downloads<<
					". If **.size==1 in omnetpp and all links has 0 delay, this "<<
					" is an error. Otherwise, it is not. In the latter case, ignore "<<
					"this message";
				debug_message(__FILE__,__LINE__,ermsg.str().c_str() );

			}
		#endif
		//</aa>

		//Output per file statistics
		sprintf ( name, "hdistance[%d].%s", getNodeIndex(), type);
		cOutVector distance_vector(name);

		for (name_t f = 1; f <= __file_bulk; f++)
			distance_vector.recordWithTimestamp(f, client_stats[f].avg_distance);
    }
}


void client::handle_timers(cMessage *timer){
    switch(timer->getKind())
	{
		case ARRIVAL:
			request_file();
			scheduleAt( simTime() + exponential(1/lambda), arrival );
			break;
		case TIMER:
			for (multimap<name_t, download >::iterator i = current_downloads.begin();i != current_downloads.end();i++)
			{
				if ( simTime() - i->second.last > RTT )
				{
					//<aa>
					#ifdef SEVERE_DEBUG
						chunk_t chunk = 0; 	// Allocate chunk data structure. 
											// This value wiil be overwritten soon
						name_t object_name = i->first;
						chunk_t object_id = __sid(chunk, object_name);
						const char* type = par("type").stringValue();

						std::stringstream ermsg; 
						ermsg<<"Client of type "<< type <<" attached to node "<< getNodeIndex() <<" was not able to retrieve object "
							<<object_id<< " before the timeout expired. Serial number of the interest="<< 
							i->second.serial_number <<". This is not necessarily a bug. If you expect "<<
							"such an event and you think it is not a bug, disable this error message";
						severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
					#endif
					//</aa>
			
					//resend the request for the given chunk
					cout<<getIndex()<<"]**********Client timer hitting ("<<simTime()-i->second.last<<")************"<<endl;
					cout<<i->first<<"(while waiting for chunk n. "<<i->second.chunk << ",of a file of "<< __size(i->first) 
						<<" chunks at "<<simTime()<<")"<<endl;
					resend_interest(i->first,i->second.chunk, i->second.repr_mask, -1);
				}
			}
			scheduleAt( simTime() + check_time, timer );
			break;

		#ifdef SEVERE_DEBUG
		default:
				std::stringstream ermsg; 
				ermsg<<"Client attached to node "<< getNodeIndex() <<" received an unrecognized message ";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		#endif
    }
}



//Generate interest requests 
void client::request_file()
{
	//<aa>
	#ifdef SEVERE_DEBUG
		if (!active){
			std::stringstream ermsg; 
			ermsg<<"Client attached to node "<< getNodeIndex() <<" is requesting file but it "
				<<"shoud not as it is not active";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif
	//</aa>

    name_t object_id = content_distribution::zipf.value(dblrand());
	cnumber_t chunk_num = 0;
	representation_mask_t repr_mask = 0xFFFF;	// We fill the representation mask with all 1s, meaning that,
												// when the client requests some object, it accepts
												// all the possible representations
	request_specific_chunk(object_id, chunk_num, repr_mask);
}

void client::request_specific_chunk_from_another_class(name_t object_id, cnumber_t chunk_num, representation_mask_t repr_mask)
{
	Enter_Method("Requesting a chunk"); // If you do not add this invocation and you call this 
										// method from another C++ class, an error will raise.
										// Search the manual for "Enter_Method" for more information
	request_specific_chunk(object_id, chunk_num, repr_mask);
}

void client::request_specific_chunk(name_t object_id, cnumber_t chunk_num, representation_mask_t repr_mask)
{
	struct download new_download = download (0,simTime(), repr_mask );
	
	#ifdef SEVERE_DEBUG
		new_download.serial_number = interests_sent;
	#endif

    current_downloads.insert(pair<name_t, download >(object_id, new_download ) );
	int toward = -1;
    send_interest(object_id, chunk_num ,repr_mask, toward);
}

void client::resend_interest(name_t name,cnumber_t number, representation_mask_t repr_mask, int toward)
{
    chunk_t chunk = 0;
    ccn_interest* interest = new ccn_interest("interest",CCN_I);
    __sid(chunk, name);
    __schunk(chunk, number);
	__srepresentation_mask(chunk, repr_mask);

    interest->setChunk(chunk);
    interest->setHops(-1);
    interest->setTarget(toward);
    interest->setNfound(true);
    send(interest, "client_port$o");


    //<aa>
    #ifdef SEVERE_DEBUG
	std::stringstream ermsg; 
	ermsg<<"An error occurred. This is not necessarily a bug. If you "<<
		"think that a retransmission is plausible in your scenario, "<<
		"please disable this error and run again";
	severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
    #endif
    //</aa>
}

void client::send_interest(name_t name,cnumber_t number, representation_mask_t repr_mask, int toward)
{
    chunk_t chunk = 0;
    ccn_interest* interest = new ccn_interest("interest",CCN_I);

    __sid(chunk, name);
    __schunk(chunk, number);
	__srepresentation_mask(chunk, repr_mask);

    interest->setChunk(chunk);
    interest->setHops(-1);
    interest->setTarget(toward);
	

	//<aa>
	#ifdef SEVERE_DEBUG
	interest->setSerialNumber(interests_sent);
	interest->setOrigin( getNodeIndex() );
	interests_sent++;
	#endif
	//</aa>

    send(interest, "client_port$o");
}



void client::handle_incoming_chunk (ccn_data *data_message)
{
    name_t object_id      = data_message -> get_object_id();
    cnumber_t chunk_num = data_message -> get_chunk_num();
	representation_mask_t repr_mask = data_message->get_representation_mask();
    filesize_t size      = data_message -> get_size();

	//<aa>
	#ifdef SEVERE_DEBUG
		if ( !is_waiting_for(object_id) )
		{
			std::stringstream ermsg; 
			ermsg<<"Client of type "<< getModuleType() <<" attached to node "<< getNodeIndex() <<" is receiving object "
				<<object_id<<" but it is not waiting for it" <<endl;
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		

		if (client_stats == 0)
		{
			std::stringstream ermsg; 
			ermsg<<"The pointer client_stats points to 0. This is an error";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif
	//</aa>


    //----------Statistics-----------------
    // Average clients statistics
    avg_distance = (tot_chunks*avg_distance+data_message->getHops())/(tot_chunks+1);
    //tot_chunks++;
    tot_downloads+=1./size;

    // File statistics. Doing statistics for all files would be tremendously
    // slow for huge catalog size, and at the same time quite useless
    // (statistics for the 12345234th file are not so meaningful at all)
	// <aa> Therefore, we compute statistics only for the most popular files </aa>
    if (object_id <= __file_bulk)
	{
        client_stats[object_id].avg_distance = 
				( client_stats[object_id].tot_chunks*client_stats[object_id].avg_distance+data_message->getHops() )/
				( client_stats[object_id].tot_chunks+1 );
        client_stats[object_id].tot_chunks++;
        client_stats[object_id].tot_downloads+=1./size;
    }




    //-----------Handling downloads------
    //Handling the download list (TODO put this piece of code within a virtual method, in this way implementing new strategies should be direct).
    pair< multimap<name_t, download>::iterator, multimap<name_t, download>::iterator > ii;
    multimap<name_t, download>::iterator it; 

    ii = current_downloads.equal_range(object_id);
    it = ii.first;

    while (it != ii.second)
	{
        if ( it->second.chunk == chunk_num && (it->second.repr_mask & repr_mask) != 0x0000 )
		{
			// We were waiting for such a chunk

			#ifdef SEVERE_DEBUG
				if ( is_it_proactive_component() && it->second.repr_mask == 0x0001 )
				{
					std::stringstream ermsg; 
					ermsg<<"A proactice component should never ask for the lowest representations";
					severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
				}
			#endif

            it->second.chunk++;
            if (it->second.chunk< __size(object_id) )
			{ 
		    	it->second.last = simTime();
		    	//if the file is not yet completed send the next interest
		    	send_interest(object_id, it->second.chunk, repr_mask, data_message->getTarget());
            }else{ 
	        	//if the file is completed delete the entry from the pendent file list
				simtime_t completion_time = simTime()-it->second.start;
				avg_time = (tot_chunks * avg_time + completion_time ) * 1./( tot_chunks+1 );
		    	if (current_downloads.count(object_id)==1)
				{
		    	    current_downloads.erase(object_id);
		    	    break;
		    	}else{
		    	    current_downloads.erase(it++);
		    	    continue;
		    	}
			}
        }
        ++it;
    }
    tot_chunks++;
}

void client::clear_stat()
{
	#ifdef SEVERE_DEBUG
		if (client_stats == 0)
		{
			std::stringstream ermsg; 
			ermsg<<"The pointer client_stats points to 0. This is an error";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif


    avg_distance = 0;
    avg_time = 0;
    tot_downloads = 0;
    tot_chunks = 0;

    //<aa>
    #ifdef SEVERE_DEBUG
	    interests_sent = 0;
    #endif
    //</aa>

    delete [] client_stats;
    client_stats = new client_stat_entry[__file_bulk+1];

	#ifdef SEVERE_DEBUG
		if (client_stats == 0)
		{
			std::stringstream ermsg; 
			ermsg<<"The pointer client_stats points to 0. This is an error";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif
}

//<aa> Get functions
bool client::is_it_proactive_component()
{
	return is_it_proactive_component_;
}

double client::get_avg_distance(){
	return avg_distance;
}
double client::get_tot_downloads(){
	return tot_downloads;
}
simtime_t client::get_avg_time(){
	return avg_time;
}
bool client::is_active(){
	return active;
}

#ifdef SEVERE_DEBUG
unsigned int client::get_interests_sent(){
	return interests_sent;
}

bool client::is_waiting_for(name_t object_id)
{
	multimap<name_t, download>::iterator it = current_downloads.find(object_id);
	return it != current_downloads.end();
}
#endif
//</aa>
