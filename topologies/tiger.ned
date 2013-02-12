package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network tiger_network {

    parameters:
    	int n = 22;

        //Number of clients
	//string node_clients = "1,2,3,4,5,6,7,8";
	string node_clients = "1";

	//Number of repositories
//	string node_repos = "11,9,18,16,13";
	string node_repos = "0,1,2,3,4,5,11,9,18,16,13,12";
	int num_repos = 12;
	int replicas = 3;
	
	//string node_repos = "13,10";
	//int num_repos = 2;
	//int replicas = 1;

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
	node[0].face++ <--> { delay = 0.15ms; } <--> node[11].face++;
	node[0].face++ <--> { delay = 0.30ms; } <--> node[12].face++;
	node[0].face++ <--> { delay = 0.17ms; } <--> node[18].face++;
	node[0].face++ <--> { delay = 0.13ms; } <--> node[19].face++;
	node[1].face++ <--> { delay = 0.15ms; } <--> node[10].face++;
	node[1].face++ <--> { delay = 0.30ms; } <--> node[2].face++;
	node[1].face++ <--> { delay = 0.18ms; } <--> node[8].face++;
	node[10].face++ <--> { delay = 0.027ms; } <--> node[11].face++;
	node[11].face++ <--> { delay = 0.027ms; } <--> node[12].face++;
	node[12].face++ <--> { delay = 0.023ms; } <--> node[13].face++;
	node[13].face++ <--> { delay = 0.020ms; } <--> node[14].face++;
	node[14].face++ <--> { delay = 0.04ms; } <--> node[15].face++;
	node[15].face++ <--> { delay = 0.012ms; } <--> node[16].face++;
	node[16].face++ <--> { delay = 0.012ms; } <--> node[17].face++;
	node[17].face++ <--> { delay = 0.012ms; } <--> node[18].face++;
	node[18].face++ <--> { delay = 0.023ms; } <--> node[19].face++;
	node[18].face++ <--> { delay = 0.020ms; } <--> node[20].face++;
	node[19].face++ <--> { delay = 0.027ms; } <--> node[20].face++;
	node[19].face++ <--> { delay = 0.012ms; } <--> node[21].face++;
	node[2].face++ <--> { delay = 0.3ms; } <--> node[12].face++;
	node[2].face++ <--> { delay = 0.3ms; } <--> node[13].face++;
	node[2].face++ <--> { delay = 0.2ms; } <--> node[3].face++;
	node[3].face++ <--> { delay = 0.23ms; } <--> node[14].face++;
	node[3].face++ <--> { delay = 0.23ms; } <--> node[15].face++;
	node[3].face++ <--> { delay = 0.47ms; } <--> node[4].face++;
	node[4].face++ <--> { delay = 0.33ms; } <--> node[16].face++;
	node[4].face++ <--> { delay = 0.33ms; } <--> node[17].face++;
	node[4].face++ <--> { delay = 0.4ms; } <--> node[5].face++;
	node[5].face++ <--> { delay = 0.25ms; } <--> node[16].face++;
	node[5].face++ <--> { delay = 0.25ms; } <--> node[17].face++;
	node[5].face++ <--> { delay = 0.33ms; } <--> node[6].face++;
	node[6].face++ <--> { delay = 0.18ms; } <--> node[16].face++;
	node[6].face++ <--> { delay = 0.18ms; } <--> node[17].face++;
	node[6].face++ <--> { delay = 0.27ms; } <--> node[7].face++;
	node[7].face++ <--> { delay = 0.13ms; } <--> node[20].face++;
	node[7].face++ <--> { delay = 0.33ms; } <--> node[8].face++;
	node[8].face++ <--> { delay = 0.15ms; } <--> node[21].face++;
	node[8].face++ <--> { delay = 0.15ms; } <--> node[9].face++;
	node[9].face++ <--> { delay = 0.012ms; } <--> node[10].face++;
	node[9].face++ <--> { delay = 0.012ms; } <--> node[21].face++;

	//node[1].face++ <--> { delay = 1ms; } <--> client[0].client_port;

	for i = 0..n-1{
	    node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
	}

}
