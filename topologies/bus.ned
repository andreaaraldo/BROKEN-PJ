package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network bus_network {

    parameters:
        //Number of ccn nodes
    	int n = default(4);

	//Number of repositories
	string node_repos = "3";//n-1
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

    for  i = 0..n-2{
	node[i].face++ <--> {delay = 1ms;} <--> node[i+1].face++;
    }

    for i = 1..n-1{
	node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
    }

}
