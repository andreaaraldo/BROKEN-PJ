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
#include "strategy_layer.h"


void strategy_layer::initialize(){
    populate_routing_table(); //Building forwarding table 

}

void strategy_layer::finish(){
    ;
}

//Common to each strategy layer. Populate the host-centric routing table.
//That comes from a centralized process based on the ctopology class.
void strategy_layer::populate_routing_table(){

    deque<int> p;
    cTopology topo;
    vector<string> types;

    //Extract topology map
    types.push_back("modules.node.node");
    topo.extractByNedTypeName( types );
    cTopology::Node *node = topo.getNode( getParentModule()->getIndex() ); //iterator node

    int rand_out;
    //As the node topology is defined as a vector of nodes (see Omnet++ manual), cTopology 
    //associates the node i with the node whose Index is i.
    for (int d = 0; d < topo.getNumNodes(); d++){
	if (d!=getParentModule()->getIndex()){

	    cTopology::Node *to   = topo.getNode( d ); //destination node
	    topo.weightedMultiShortestPathsTo( to ); 
	    rand_out = node->getNumPaths() == 1 ? 0 : intrand (node->getNumPaths()-1);

	    FIB[d].id = node->getPath(rand_out)->getLocalGate()->getIndex();
	    FIB[d].len = node->getDistanceToTarget();
	}
    }

}






