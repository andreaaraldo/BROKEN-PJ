package topologies;
import modules.*;


network grid_network {

    parameters:
        //Number of ccn nodes
    	int n = 16;

	//Number of repositories
	string node_repos = default("0");
	int num_repos = default(1);
	int replicas = default(1);

        //Number of clients
	string node_clients = default("1");
	int num_clients = default(1);


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

    node[0].face++ <--> { delay = 1ms; } <--> node[1].face++;
    node[1].face++ <--> { delay = 1ms; } <--> node[2].face++;
    node[2].face++ <--> { delay = 1ms; } <--> node[3].face++;
    node[4].face++ <--> { delay = 1ms; } <--> node[5].face++;
    node[5].face++ <--> { delay = 1ms; } <--> node[6].face++;
    node[6].face++ <--> { delay = 1ms; } <--> node[7].face++;
    node[8].face++ <--> { delay = 1ms; } <--> node[9].face++;
    node[9].face++ <--> { delay = 1ms; } <--> node[10].face++;
    node[10].face++ <--> { delay = 1ms; } <--> node[11].face++;
    node[12].face++ <--> { delay = 1ms; } <--> node[13].face++;
    node[13].face++ <--> { delay = 1ms; } <--> node[14].face++;
    node[14].face++ <--> { delay = 1ms; } <--> node[15].face++;
    node[0].face++ <--> { delay = 1ms; } <--> node[4].face++;
    node[4].face++ <--> { delay = 1ms; } <--> node[8].face++;
    node[8].face++ <--> { delay = 1ms; } <--> node[12].face++;
    node[1].face++ <--> { delay = 1ms; } <--> node[5].face++;
    node[5].face++ <--> { delay = 1ms; } <--> node[9].face++;
    node[9].face++ <--> { delay = 1ms; } <--> node[13].face++;
    node[2].face++ <--> { delay = 1ms; } <--> node[6].face++;
    node[6].face++ <--> { delay = 1ms; } <--> node[10].face++;
    node[10].face++ <--> { delay = 1ms; } <--> node[14].face++;
    node[3].face++ <--> { delay = 1ms; } <--> node[7].face++;
    node[7].face++ <--> { delay = 1ms; } <--> node[11].face++;
    node[11].face++ <--> { delay = 1ms; } <--> node[15].face++;
    node[11].face++ <--> { delay = 10ms; } <--> node[0].face++;

    for i = 0..n-1{
	node[i].face++ <--> { delay = 1ms; } <--> client[i].client_port;
    }

	    

}
