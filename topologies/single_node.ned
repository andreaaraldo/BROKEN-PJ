package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network single_network {

    parameters:
        //Number of ccn nodes
    	int n = 1;

	//Number of repositories
	string node_repos = "0";//n-1
	int num_repos = 1;
	int replicas = 1;

	//Number of clients 
	int num_clients = 1;
	string node_clients = "0";

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
    node[0].face++ <--> { delay = 1ms; } <--> client[0].client_port;
}
