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
#include "content/content_distribution.h"

#include "statistics/statistics.h"

#include "packets/ccn_interest.h"
#include "packets/ccn_data.h"

#include "core/definitions.h"
#include "client.h"

Register_Class (client);


void client::initialize(){

    //Parameters initialization
    n               = getAncestorPar("n");
    lambda          = par ("lambda");
    RTT             = par("RTT");
    check_time      = par("check_time");
    int num_clients = getAncestorPar("num_clients");


    //Allocating file statistics
    client_stats = new client_stat_entry[content_distribution::perfile_bulk + 1];


    //Initialize average stats
    avg_distance = 0;
    avg_time = 0;
    tot_downloads = 0;
    tot_chunks = 0;
	

    if (find(content_distribution::clients , content_distribution::clients + num_clients ,getNodeIndex()) 
	    != content_distribution::clients + num_clients){
	active = true;

	scheduleAt( simTime() , new cMessage("arrival", ARRIVAL ) );
	scheduleAt( simTime() + check_time, new cMessage("timer", TIMER) );

    }

}


void client::handleMessage(cMessage *in){
   
    if (in->isSelfMessage()){
	handle_timers(in);
	return;
    }

    switch (in->getKind()){
	case CCN_D:
	{
	    ccn_data *data_message = (ccn_data *) in;
	    handle_incoming_chunk (data_message);
	}
    }
    delete in;


}

int client::getNodeIndex(){
    return gate("client_port$o")->getNextGate()->getOwnerModule()->getIndex();

}

void client::finish(){
    //Output average local statistics
    char name [30];
    sprintf ( name, "avg_distance[%d]", getNodeIndex());
    recordScalar (name, avg_distance);

    sprintf ( name, "downloads[%d]",getNodeIndex());
    recordScalar (name, tot_downloads );

    //Output per file statistics
    sprintf ( name, "node[%d]", getNodeIndex());
    cOutVector distance_vector(name);
    for (uint32_t f = 1; f <= content_distribution::perfile_bulk; f++)
        distance_vector.recordWithTimestamp(f, client_stats[f].avg_distance);
}


void client::handle_timers(cMessage *timer){
    switch(timer->getKind()){
	case ARRIVAL:
	    //cout<<getNodeIndex()<<"] arrival at " << simTime()<< endl;
	    request_file();

	    scheduleAt( simTime() + exponential(1/lambda), new cMessage("arrival", ARRIVAL ) );
	    delete timer;
	    break;
	case TIMER:
	    for (multimap<uint32_t,file_entry>::iterator i = current_downloads.begin();i != current_downloads.end();i++){
		if ( simTime() - i->second.last > RTT ){
		    //resend the request for the given chunk
		    send_interest(i->first,i->second.missing_chunks,-1);
		    cout<<getIndex()<<"]**********Client timer hitting ("<<simTime()-i->second.last<<")************"<<endl;
		    cout<<i->first<<"(waiting for chunk n. "<<i->second.missing_chunks<<",of a file of "<< __size(i->first) <<" chunks at "<<simTime()<<")"<<endl;
		}
	    }
	    scheduleAt( simTime() + check_time, new cMessage("timer", TIMER) );
	    delete timer;
	    break;


    }
}



//Generate interest requests 
void client::request_file(){

    uint32_t name = content_distribution::zipf.value(dblrand());
    //uint32_t name =1;
    cout<<"client request for "<<name<<endl;
    current_downloads.insert(pair<uint32_t,file_entry>(name, file_entry (0,simTime() ) ) );
    send_interest(name, 0 ,-1);

}

void client::send_interest(name_t name,cnumber_t number, int toward){
    uint64_t chunk = 0;
    ccn_interest* interest = new ccn_interest("interest",CCN_I);

    __sid(chunk, name);
    __schunk(chunk, number);

    interest->setChunk(chunk);
    interest->setHops(-1);
    interest->setTarget(toward);
    send(interest, "client_port$o");
}



void client::handle_incoming_chunk (ccn_data *data_message){

    pair< multimap<uint32_t,file_entry>::iterator, multimap<uint32_t,file_entry>::iterator > ii;
    multimap<uint32_t, file_entry>::iterator it; 

    uint32_t chunk_num = data_message -> get_chunk_num();
    uint32_t name      = data_message -> get_name();
    uint32_t size      = data_message -> get_size();

    // Average node statistics
    avg_distance = (tot_chunks*avg_distance+data_message->getHops())/(tot_chunks+1);
    tot_chunks++;
    tot_downloads+=1./size;


    // File statistics. Doing statistics for all files would be tremendously
    // slow for huge catalog size, and at the same time quite useless
    // (statistics for the 12345234th file are not so meaningful at all)
    if (name <= content_distribution::perfile_bulk){
	client_stats[name].avg_distance = (client_stats[name].tot_chunks*avg_distance+data_message->getHops())/(client_stats[name].tot_chunks+1);
	client_stats[name].tot_chunks++;
	client_stats[name].tot_downloads+=1./size;
    }




    //Handling the download list (TODO put this piece of code within a virtual method, in this way implementing new strategies should be direct).
    ii = current_downloads.equal_range(name);
    it = ii.first;

    while (it != ii.second){
        if ( it->second.missing_chunks == chunk_num ){
            it->second.missing_chunks++;

            if (it->second.missing_chunks < __size(name) ){ //if the file is not yet completed
		it->second.last = simTime();
        	send_interest(name, it->second.missing_chunks,data_message->getTarget());
            } else { //if the file is completed

        	//Delete the entry from the pendent file list
        	if (current_downloads.count(name)==1){
        	    current_downloads.erase(name);
        	    break;
        	} else{
        	    current_downloads.erase(it++);
		    continue;
		}
            }
        }
	++it;
    }


}

void client::clear_stat(){
    avg_distance = 0;
    avg_time = 0;
    tot_downloads = 0;
    tot_chunks = 0;
    delete client_stats;
    client_stats = new client_stat_entry[content_distribution::perfile_bulk + 1];
}
