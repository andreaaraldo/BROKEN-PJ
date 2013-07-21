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
#include <sstream>
ifstream strategy_layer::fdist;
ifstream strategy_layer::frouting;


void strategy_layer::initialize(){

    for (int i = 0; i<getParentModule()->gateSize("face$o");i++){
	int index ;
	if (!check_client(i))
	    index = getParentModule()->gate("face$o",i)->getNextGate()->getOwnerModule()->getIndex();
        gatelu[index] = i;
    }
    
    
    string fileradix = par("routing_file").stringValue();
    string filerout = fileradix+".rou";
    string filedist = fileradix+".dist";
    if (fileradix!= ""){
	if (!fdist.is_open()){
	    fdist.open(filedist.c_str());
	    frouting.open(filerout.c_str());
	}
	populate_from_file(); //Building forwarding table 
    }else 
	populate_routing_table(); //Building forwarding table 
}

void strategy_layer::finish(){
    fdist.close();
    frouting.close();
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
	    rand_out = node->getNumPaths() == 1 ? 0 : intrand (node->getNumPaths());

	    FIB[d].id = node->getPath(rand_out)->getLocalGate()->getIndex();
	    FIB[d].len = node->getDistanceToTarget();
	    //cout<<getParentModule()->gate("face$o",FIB[d].id)->getNextGate()->getOwnerModule()->getIndex()+1<<" ";
	    //cout<<FIB[d].len<<" ";
	}else
	    ;//cout<<getParentModule()->getIndex()+1<<" ";
	    //cout<<0<<" ";
    }
    //cout<<FIB[0].len+1<<";"<<endl;
    //cout<<getParentModule()->gate("face$o",FIB[0].id)->getNextGate()->getOwnerModule()->getIndex()+1<<";"<<endl;;

    //if (getIndex()==0){
    //    cout<<"}}";
    //    for (int d = 0; d<topo.getNumNodes();d++){
    //        cout<<FIB[d].len+1<<" ";
    //    }
    //    cout<<0<<";"<<endl;;
    //}

    //cout<<";"<<endl;

}

void strategy_layer::populate_from_file(){
    string rline, dline;
    getline(frouting, rline);
    getline(fdist, dline);

    istringstream diis (dline);
    istringstream riis (rline);
    int n = getAncestorPar("n");

    int cell;
    int k = 0;

    while (k<n){
	riis>>cell;
	FIB[k++].id = gatelu[cell-1];
    }

    k = 0;
    while (k<n){
	diis>>cell;
	FIB[k++].len = cell;
    }

}
