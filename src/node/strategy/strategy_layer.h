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
#ifndef STRATEGY_H_
#define STRATEGY_H_

#include <omnetpp.h>

#include "node/abstract_node.h"
#include <boost/unordered_map.hpp>

using namespace std;
using namespace boost;

struct int_f{
    unsigned short id;
    unsigned short len;
};


//
//Basic strategy layer class. In order to "be" a strategy layer 
//a class needs to define its own get_decision function
//which returns an array of booleans. The i-th bool value
//indicates if the message should be sent on the i-th interface.
//
class strategy_layer: public AbstractNode {
    public:
	//The only interface function. Cores should be call this function in
	//order to get the interfaces on which sending the current interest
	virtual bool* get_decision(cMessage *)=0;
    protected:
	//Omnet base functions
	virtual void initialize();
	virtual void finish();

	//FIB initialization functions
	void populate_routing_table();

	//FIB (available to all subclasses, for sake of utilization)
	unordered_map <uint32_t,int_f> FIB;
	int nodes;

};
#endif
