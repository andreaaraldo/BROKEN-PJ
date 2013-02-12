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
#ifndef CLIENT_H_
#define CLIENT_H_

#include <omnetpp.h>
#include <boost/unordered_map.hpp>
#include "statistics/statistics.h"
#include "packets/ccn_data.h"

using namespace boost;
using namespace std;



//Each of these entries contains information about the current downloads
struct file_entry{
    uint32_t missing_chunks;

    simtime_t start;
    simtime_t last;

    file_entry(double m = 0,simtime_t t = 0):missing_chunks(m),start(t),last(t){;}
};

//Each of these entries contains information about statistics for each single file
struct client_stat_entry{
    double avg_distance;
    double avg_time;
    uint64_t tot_downloads;
    uint64_t tot_chunks;

    client_stat_entry():avg_distance(0),avg_time(0),tot_downloads(0),tot_chunks(0){;}

};



class client : public cSimpleModule {
    friend class statistics;
    protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

	virtual void handle_incoming_chunk(ccn_data *);
	virtual void request_file();
	void send_interest(name_t, cnumber_t, int);

	void handle_timers(cMessage*);

	int  getNodeIndex();
	bool check_full();

	void clear_stat();


    private:

	//List of current downloads for a given file
	multimap <uint32_t, file_entry> current_downloads;

	//Single file statistics
	client_stat_entry* client_stats;

	//Average statistics
	double tot_downloads;
	uint64_t tot_chunks;

	double avg_time;
	double avg_distance;



	//INI parameters
	double lambda;
	uint32_t n;
	double RTT;
	double check_time;

	//Set if the client actively sends interests for files
	bool active;


};

#endif
