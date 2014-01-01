package networks;

import modules.node.node;
import modules.channels.IcnChannel;

//
// Two instances (tic and toc) of Txc connected.
//
network simple_scenario_cache_at_border_network extends base_network
{
    parameters:
        //Number of ccn nodes
    	n = 5;
		num_clients = 1;
		node_clients = "0";
		num_repos = 2;
		node_repos = "3,4";

		**.cost = 300;
		
		int cachesize;

		//TODO: convert node[0] and others into TransparentNode
		node[0].content_store.C =  0;
		node[1].content_store.C =  cachesize;
		node[2].content_store.C =  0;
		node[3].content_store.C =  0;
		node[4].content_store.C =  0;



    connections allowunconnected :
    	// Remember that the first port of each node is connected to a client 
		// (see networks/base.ned), even if the client is not active
        node[0].face++ <-->  IcnChannel <--> node[1].face++;
        node[0].face++ <-->  IcnChannel <--> node[2].face++;
        node[1].face++ <-->  IcnChannel{cost_desc=1;} <--> node[3].face++;
        node[2].face++ <-->  IcnChannel <--> node[4].face++;
}
