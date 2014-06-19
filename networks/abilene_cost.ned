package networks;

import modules.channels.IcnChannel;


network abilene_cost_network extends base_network{

    parameters:
        //Number of ccn nodes
		int num_caching_nodes = 11; // All nodes in 0..num_caching_nodes-1 are caching
    	n = num_caching_nodes+3; 	//The 3 additional nodes are fake nodes to which
					//we will attach the repos

		int priceratio;
		double xi;
		int cachesize;
		string decision_policy;


		double cheap_price = 1;
		double expensive_price = priceratio;
		num_clients = 2;
		node_clients = "0,11";
		num_repos = 3;
		node_repos = "11,12,13";


		int single_cachesize = ceil(cachesize / num_caching_nodes );
		node[0..10].content_store.C = single_cachesize;
		node[0..10].content_store.DS = decision_policy;


		// The following nodes are "fake"
		node[11..13].content_store.C =  0;
		node[11..13].core_layer.interest_aggregation =  false; 
		node[11..13].core_layer.transparent_to_hops = true;
		node[11..13].content_store.DS = "never";

connections allowunconnected:
	node[1].face++ <--> { delay = 0.00001ms; } <--> node[0].face++;
	node[10].face++ <--> { delay = 0.00001ms; } <--> node[0].face++;
	node[10].face++ <--> { delay = 0.00001ms; } <--> node[1].face++;
	node[10].face++ <--> { delay = 0.00001ms; } <--> node[9].face++;
	node[2].face++ <--> { delay = 0.00001ms; } <--> node[1].face++;
	node[3].face++ <--> { delay = 0.00001ms; } <--> node[2].face++;
	node[4].face++ <--> { delay = 0.00001ms; } <--> node[3].face++;
	node[5].face++ <--> { delay = 0.00001ms; } <--> node[4].face++;
	node[6].face++ <--> { delay = 0.00001ms; } <--> node[5].face++;
	node[7].face++ <--> { delay = 0.00001ms; } <--> node[3].face++;
	node[7].face++ <--> { delay = 0.00001ms; } <--> node[6].face++;
	node[8].face++ <--> { delay = 0.00001ms; } <--> node[2].face++;
	node[8].face++ <--> { delay = 0.00001ms; } <--> node[7].face++;
	node[9].face++ <--> { delay = 0.00001ms; } <--> node[8].face++;

	// Attach the repos
	node[0].face++ <--> IcnChannel{price_desc=0; max_price=expensive_price; delay = 0.00001ms; } <-->node[11].face++;
	node[5].face++ <--> IcnChannel{price_desc=cheap_price; max_price=expensive_price; delay = 0.00001ms; } <-->node[12].face++;
	node[9].face++ <--> IcnChannel{price_desc=expensive_price; max_price=expensive_price; delay = 0.00001ms; } <-->node[13].face++;
}
