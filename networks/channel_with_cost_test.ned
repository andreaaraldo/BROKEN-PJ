package networks;

import modules.node.node;
import modules.channels.IcnChannel;

//
// Two instances (tic and toc) of Txc connected.
//
network channel_with_cost_test_network extends base_network
{
    parameters:
        //Number of ccn nodes
    	n = 3;
		//**.num_clients = 1;
		//node[0].face$o[1].channel.cost = 200;
		**.cost = 0;

    connections allowunconnected :
    	//Remember that the first port of each node is connected to a client (see networks/base.ned)
        node[0].face++ <-->  IcnChannel <--> node[1].face++; //face 1 of node[0], face 1 of node[1]
        node[1].face++ <-->  IcnChannel <--> node[2].face++; //face 2 of node[1], face 1 of node[0]
}
