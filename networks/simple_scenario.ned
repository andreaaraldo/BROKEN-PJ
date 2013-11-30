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

		**.cost = 0;
		node[0].face$o[1].channel.cost = 300;

		node[0].content_store.C =  0;
		node[1].content_store.C =  10^2;
		node[2].content_store.C =  10^2;
		node[3].content_store.C =  0;


    connections allowunconnected :
    	//Remember that the first port of each node is connected to a client (see networks/base.ned),
    	// even if the client is not active
        node[0].face++ <-->  IcnChannel <--> node[1].face++; //face 1 of node[0], face 1 of node[1]
        node[0].face++ <-->  IcnChannel <--> node[2].face++; //face 2 of node[0], face 1 of node[2]
        node[1].face++ <-->  IcnChannel <--> node[3].face++; //face 2 of node[1], face 1 of node[3]
        node[2].face++ <-->  IcnChannel <--> node[3].face++; //face 2 of node[2], face 2 of node[3]
}
