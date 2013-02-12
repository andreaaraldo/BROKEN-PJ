package topologies;
import modules.*;

network onion_network {

    parameters:
        //Number of ccn nodes
    	int n = 23;

        //Number of clients
	string node_clients = "0,1,2,3,4";

	//Number of repositories
	string node_repos = "0,4,17,14,12";
	int num_repos = 5;
	int replicas = 3;

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

	//Level 0 has 5 nodes, namely:
	node[0].face++ <--> { delay = 1ms; } <--> node[1].face++;
	node[1].face++ <--> { delay = 1ms; } <--> node[2].face++;
	node[2].face++ <--> { delay = 1ms; } <--> node[3].face++;
	node[3].face++ <--> { delay = 1ms; } <--> node[4].face++;
	node[4].face++ <--> { delay = 1ms; } <--> node[0].face++;
	//Level 1 has 8 nodes, namely:
	node[5].face++ <--> { delay = 1ms; } <--> node[6].face++;
	node[6].face++ <--> { delay = 1ms; } <--> node[7].face++;
	node[7].face++ <--> { delay = 1ms; } <--> node[8].face++;
	node[8].face++ <--> { delay = 1ms; } <--> node[9].face++;
	node[9].face++ <--> { delay = 1ms; } <--> node[10].face++;
	node[10].face++ <--> { delay = 1ms; } <--> node[11].face++;
	node[11].face++ <--> { delay = 1ms; } <--> node[12].face++;
	node[12].face++ <--> { delay = 1ms; } <--> node[5].face++;
	//Level 2 has 10 nodes, namely:
	node[13].face++ <--> { delay = 1ms; } <--> node[14].face++;
	node[14].face++ <--> { delay = 1ms; } <--> node[15].face++;
	node[15].face++ <--> { delay = 1ms; } <--> node[16].face++;
	node[16].face++ <--> { delay = 1ms; } <--> node[17].face++;
	node[17].face++ <--> { delay = 1ms; } <--> node[18].face++;
	node[18].face++ <--> { delay = 1ms; } <--> node[19].face++;
	node[19].face++ <--> { delay = 1ms; } <--> node[20].face++;
	node[20].face++ <--> { delay = 1ms; } <--> node[21].face++;
	node[21].face++ <--> { delay = 1ms; } <--> node[22].face++;
	node[22].face++ <--> { delay = 1ms; } <--> node[13].face++;

	//Level 1 and 0 connections
	//With 3/(8+5)= 0.230769230769231 conn. probability
	node[6].face++ <--> { delay = 1ms; } <--> node[1].face++;
	node[6].face++ <--> { delay = 1ms; } <--> node[3].face++;
	node[7].face++ <--> { delay = 1ms; } <--> node[1].face++;
	node[9].face++ <--> { delay = 1ms; } <--> node[1].face++;
	node[10].face++ <--> { delay = 1ms; } <--> node[0].face++;
	node[11].face++ <--> { delay = 1ms; } <--> node[1].face++;
	node[11].face++ <--> { delay = 1ms; } <--> node[4].face++;

	//Level 2 and 1 connections
	//With 3/(10+8)= 0.166666666666667 conn. probability
	node[13].face++ <--> { delay = 1ms; } <--> node[11].face++;
	node[13].face++ <--> { delay = 1ms; } <--> node[12].face++;
	node[14].face++ <--> { delay = 1ms; } <--> node[5].face++;
	node[14].face++ <--> { delay = 1ms; } <--> node[7].face++;
	node[14].face++ <--> { delay = 1ms; } <--> node[8].face++;
	node[15].face++ <--> { delay = 1ms; } <--> node[5].face++;
	node[15].face++ <--> { delay = 1ms; } <--> node[9].face++;
	node[15].face++ <--> { delay = 1ms; } <--> node[12].face++;
	node[17].face++ <--> { delay = 1ms; } <--> node[5].face++;
	node[17].face++ <--> { delay = 1ms; } <--> node[11].face++;
	node[18].face++ <--> { delay = 1ms; } <--> node[11].face++;
	node[19].face++ <--> { delay = 1ms; } <--> node[9].face++;
	node[20].face++ <--> { delay = 1ms; } <--> node[5].face++;
	node[20].face++ <--> { delay = 1ms; } <--> node[6].face++;
	node[20].face++ <--> { delay = 1ms; } <--> node[7].face++;
	node[20].face++ <--> { delay = 1ms; } <--> node[10].face++;
	node[21].face++ <--> { delay = 1ms; } <--> node[7].face++;
	node[21].face++ <--> { delay = 1ms; } <--> node[8].face++;
	node[21].face++ <--> { delay = 1ms; } <--> node[12].face++;

	for i = 0..n-1{
	    node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
	}
}

