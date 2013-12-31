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
    	n = 4;
		num_clients = 1;
		node_clients = "0";
		num_repos = 1;
		node_repos = "3";

		**.cost = 300;
		//node[1].face$o[2].channel.cost = 300;
		//node[2].face$o[2].channel.cost = 300;
		
		int cachesize;

		//TODO: convert node[0] and node[3] into TransparentNode
		node[0].content_store.C =  0;
		node[0].core_layer.interest_aggregation =  false; 
		node[1].content_store.C =  cachesize;
		node[2].content_store.C =  cachesize;
		node[3].content_store.C =  0;
		node[3].core_layer.interest_aggregation =  false;
		node[0].FS = "ProbabilisticSplitStrategy";
		node[0].strategy_layer.split_factors = "0 0.5 0.5";


    connections allowunconnected :
    	//Remember that the first port of each node is connected to a client (see networks/base.ned),
    	// even if the client is not active
        node[0].face++ <-->  IcnChannel <--> node[1].face++; //face 1 of node[0], face 1 of node[1]
        node[0].face++ <-->  IcnChannel <--> node[2].face++; //face 2 of node[0], face 1 of node[2]
        node[1].face++ <-->  IcnChannel{cost_desc=1;} <--> node[3].face++; //face 2 of node[1], face 1 of node[3]
        node[2].face++ <-->  IcnChannel{cost_desc=2;} <--> node[3].face++; //face 2 of node[2], face 2 of node[3]
}
