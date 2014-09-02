package networks;

import modules.channels.IcnChannel;


network abilene_cost_network extends base_network{

    parameters:
        //Number of ccn nodes
    	n = 11;

		int priceratio;
		double xi;
		int cachesize;
		string decision_policy;


		double cheap_price = 1;
		double expensive_price = priceratio;
		num_clients = 3;
		node_clients = "5,0,9";
		num_repos = 3;
		node_repos = "1,7,10";


		int single_cachesize = ceil(cachesize / n );
		node[0..10].content_store.C = single_cachesize;
		node[0..10].content_store.DS = decision_policy;

connections allowunconnected:
	node[1].face++ <--> { delay = 5.48ms; } <--> node[0].face++;
	node[10].face++ <--> { delay = 3.80ms; } <--> node[0].face++;
	node[10].face++ <--> { delay = 5.02ms; } <--> node[1].face++;
	node[10].face++ <--> { delay = 1.68ms; } <--> node[9].face++;
	node[2].face++ <--> { delay = 2.99ms; } <--> node[1].face++;
	node[3].face++ <--> { delay = 2.43ms; } <--> node[2].face++;
	node[4].face++ <--> { delay = 0.88ms; } <--> node[3].face++;
	node[5].face++ <--> { delay = 3.82ms; } <--> node[4].face++;
	node[6].face++ <--> { delay = 1.10ms; } <--> node[5].face++;
	node[7].face++ <--> { delay = 2.30ms; } <--> node[3].face++;
	node[7].face++ <--> { delay = 2.91ms; } <--> node[6].face++;
	node[8].face++ <--> { delay = 3.48ms; } <--> node[2].face++;
	node[8].face++ <--> { delay = 3.77ms; } <--> node[7].face++;
	node[9].face++ <--> { delay = 7.36ms; } <--> node[8].face++;

}
