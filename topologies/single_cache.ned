package topologies;
import modules.*;


network single_network {

    parameters:
        //Number of ccn nodes
    	int n = 1;

        //Number of clients
	int c = 1;

	//Number of repositories
	int num_repos = 1;
	string node_repos = "0";
	int replicas = 1;

    submodules:
 	statistics: statistics{
	    parameters:
	        @display("p=400, 200");
	}
    	content_distribution: content_distribution{
	    parameters:
	    	F = 1;
	        @display("p=400,300");
	}
	client[c]: client{
	    parameters:
		@display("p=80,80,m,4,50,50");
	}

        node [n]: node {
            parameters:
		@display("p=200,200,m,4,50,50");
        }

connections allowunconnected:

	node[0].face++ <--> { delay = 1ms; } <--> client[0].client_port;

}
