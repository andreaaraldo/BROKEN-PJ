package networks;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network base_network {

    parameters:
        //Number of ccn nodes
    	int n = default(1);

	//Number of repositories
	string node_repos = default("");//n-1
	int num_repos = default(1);
	int replicas = default(1);

	//Number of clients 
	int num_clients = default(1);
	string node_clients = default("");

    submodules:

    	content_distribution: content_distribution{
	    parameters:
	        @display("p=900,200");
	}

 	statistics: statistics{
	    parameters:
	        @display("p=900,100");
	}

        node [n]: node ;
	client[n]: client;



connections allowunconnected:

	for i = 0..n-1{
	    node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
	}

}
