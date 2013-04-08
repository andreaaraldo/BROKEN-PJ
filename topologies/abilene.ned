package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network abilene_network {

    parameters:
        //Number of ccn nodes
    	int n = 11;

	//Number of repositories
	string node_repos = "0";//n-1
	int num_repos = 1;
	int replicas = 1;

	//Number of clients 
	int num_clients = 1;
	string node_clients = "1";

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

	for i = 0..n-1{
	    node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
	}

}
