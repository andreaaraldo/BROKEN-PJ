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
#ifndef STATISTICS_H_
#define STATISTICS_H_
#include <omnetpp.h>
#include <ctopology.h>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

class client;
class core_layer;
class base_cache;

using namespace std;
using namespace boost;

/*
 * This class defines the central class for collecting statistics. Its first
 * goal is resetting nodes (i.e., clients, caches, and so forth) statistics in
 * order when some events occurs, like the complete filling of the caches, or
 * the stabilization of all the hit rate of all nodes. 
 *
 */
class statistics : public cSimpleModule{
    protected:
	virtual void initialize();
	//Handle message deals with timers for checking cache states and stable
	//time. No statistics are retained from this class during the
	//simulation.  Indeed, at the end it pings each component a gather the
	//final statistics(look at the method finish, below)
	virtual void handleMessage(cMessage *);
	virtual void finish();

	//Stable checks if the system is in a stable state.
	virtual bool stable(int);

	//Ask to each worth component (e.g., clients or caches)  of clearing
	//its internal statistics
	void clear_stat();


    private:

	//Vector for accessing different modules statistics
	client** clients;
	core_layer** cores;
	base_cache** caches;

	//Network infos
	int num_nodes;
	int num_clients;

	//Stabilization parameters
	double ts;
	double window;
	double partial_n;
	double time_steady;
	double stabilization_time;

	//Stabilization samples
	vector< vector <double> > samples;
	unordered_map <int, unordered_set <int> > level_union;
	unordered_map <int, int> level_same;

};
#endif
