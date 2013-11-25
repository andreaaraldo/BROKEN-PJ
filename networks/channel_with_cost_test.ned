package networks;

import modules.node.node;
import modules.channels.ChannelWithCost;

//
// Two instances (tic and toc) of Txc connected.
//
network channel_with_cost_test_network extends base_network
{
    parameters:
        //Number of ccn nodes
    	n = 3;
	//**.num_clients = 1;
	//node[0].face$o[1].channel.cost = 100;

    connections allowunconnected :
        node[0].face++ <-->  node[1].face++;
//        node[1].face++ <-->  node[2].face++;
}
