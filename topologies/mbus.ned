package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network mbus_network {

    parameters:
        //Number of ccn nodes
    	int n = 8;

	//Number of repositories
	string node_repos = "4,7";//n-1
	int num_repos = 2;
	int replicas  = 2;

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

    for  i = 1..3{
	node[i].face++ <--> {delay = 1ms;} <--> node[i+1].face++;
    }

    for  i = 1..2{
	node[i+4].face++ <--> {delay = 1ms;} <--> node[i+5].face++;
    }

    node[0].face++ <--> {delay = 1ms;} <--> node[1].face++;
    node[0].face++ <--> {delay = 1ms;} <--> node[5].face++;

    for i = 1..n-1{
	node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
    }

}
