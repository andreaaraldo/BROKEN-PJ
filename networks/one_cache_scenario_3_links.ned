package networks;

import modules.node.node;
import modules.channels.IcnChannel;

//
// Two instances (tic and toc) of Txc connected.
//
network one_cache_scenario_3_links_network extends base_network
{
    parameters:
        //Number of ccn nodes
    	n = 7;
		// n[0] is the core node, n[1] and n[2] the border nodes, n[3] and n[4] the repository nodes.
		num_clients = 1;
		node_clients = "0";
		num_repos = 3;
		node_repos = "4,5,6";
		int priceratio;
		double xi;
		
		int cachesize;
		string decision_policy;

		content_distribution.priceratio = priceratio;
		double cheap_price = 1;
		double expensive_price = priceratio;

		// The following two values are not used to compute the cost but are only used by the costprob policy
		// to take its decision
		double cheap_price_powered = 1/(1+priceratio^xi);
		double expensive_price_powered = (priceratio^xi)/(1+priceratio^xi);


		// The following nodes are "fake"
		node[1].content_store.C =  0;
		node[1].core_layer.interest_aggregation =  false; 
		node[1].core_layer.transparent_to_hops = true;
		node[2].content_store.C =  0;
		node[2].core_layer.interest_aggregation =  false; 
		node[2].core_layer.transparent_to_hops = true;
		node[3].content_store.C =  0;
		node[3].core_layer.interest_aggregation =  false; 
		node[3].core_layer.transparent_to_hops = true;
		node[4].content_store.C =  0;
		node[4].core_layer.interest_aggregation =  false; 
		node[4].core_layer.transparent_to_hops = true;
		node[5].content_store.C =  0;
		node[5].core_layer.interest_aggregation =  false; 
		node[5].core_layer.transparent_to_hops = true;
		node[6].content_store.C =  0;
		node[6].core_layer.interest_aggregation =  false; 
		node[6].core_layer.transparent_to_hops = true;


		node[0].content_store.C =  cachesize;
		node[0].content_store.DS = decision_policy;


    connections allowunconnected :
    	//Remember that the first port of each node is connected to a client (see networks/base.ned),
    	// even if the client is not active
        node[0].face++ <--> node[1].face++;
        node[0].face++ <--> node[2].face++;
        node[0].face++ <--> node[3].face++;
        node[1].face++ <-->  IcnChannel{price_desc=0; price_desc_powered=0;} <--> node[4].face++;
        node[2].face++ <-->  IcnChannel{price_desc=cheap_price;  price_desc_powered=cheap_price_powered;} <--> node[5].face++;
        node[3].face++ <-->  IcnChannel{price_desc=expensive_price;  price_desc_powered=expensive_price_powered;} <--> node[6].face++;
}
