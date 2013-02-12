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
class client;
class core_layer;
class base_cache;

using namespace std;

class statistics : public cSimpleModule{
    protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();
	virtual bool stable(int);
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

	vector< vector <double> > samples;

};
#endif
