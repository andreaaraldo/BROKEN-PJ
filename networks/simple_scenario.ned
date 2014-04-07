package networks;

import modules.node.node;
import modules.channels.IcnChannel;

//
// Two instances (tic and toc) of Txc connected.
//
network simple_scenario_network extends base_network
{
    parameters:
        //Number of ccn nodes
    	n = 5;
		// n[0] is the core node, n[1] and n[2] the border nodes, n[3] and n[4] the repository nodes.
		num_clients = 1;
		node_clients = "0";
		num_repos = 2;
		node_repos = "3,4";
		
		int cachesize;

		// The following nodes are "fake"
		node[0].content_store.C =  0;
		node[0].core_layer.interest_aggregation =  false; 
		node[3].content_store.C =  0;
		node[3].core_layer.interest_aggregation =  false; 
		node[4].content_store.C =  0;
		node[4].core_layer.interest_aggregation =  false; 

		node[1].content_store.C =  cachesize;
		node[2].content_store.C =  cachesize;


    connections allowunconnected :
    	//Remember that the first port of each node is connected to a client (see networks/base.ned),
    	// even if the client is not active
        node[0].face++ <--> node[1].face++;
        node[0].face++ <--> node[2].face++;
        node[1].face++ <-->  IcnChannel{price_desc=1;} <--> node[3].face++;
        node[2].face++ <-->  IcnChannel{price_desc=80;} <--> node[4].face++;
}
