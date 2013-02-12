package topologies;
import modules.*;


network torus_network {

    parameters:
        //Number of ccn nodes
    	int n = 16;

        //Number of clients
	int c=n;
	string node_clients = "";

	//Number of repositories
	int num_repos = default(1);
	string node_repos = default("{;}");
	int replicas = default(1);

    submodules:
 	statistics: statistics{
	    parameters:
	        @display("p=400,200");
	}
    	content_distribution: content_distribution{
	    parameters:
	        @display("p=400,300");
	}
	client[c]: client{
	    parameters:
		@display("p=80,80,m,4,50,50");
	}

        node [n]: node {
            parameters:
		@display("p=100,100,m,4,50,50");
        }

connections allowunconnected:

    node[0].face++ <--> { delay = 1ms; }<--> node[1].face++;
    node[1].face++ <--> { delay = 1ms; }<--> node[2].face++;
    node[2].face++ <--> { delay = 1ms; }<--> node[3].face++;
    node[0].face++ <--> { delay = 1ms; }<--> node[4].face++;
    node[3].face++ <--> { delay = 1ms; }<--> node[7].face++;
    node[4].face++ <--> { delay = 1ms; }<--> node[5].face++;
    node[5].face++ <--> { delay = 1ms; }<--> node[6].face++;
    node[6].face++ <--> { delay = 1ms; }<--> node[7].face++;
    node[4].face++ <--> { delay = 1ms; }<--> node[8].face++;   
    node[7].face++ <--> { delay = 1ms; }<--> node[11].face++;
    node[8].face++ <--> { delay = 1ms; }<--> node[9].face++;
    node[9].face++ <--> { delay = 1ms; }<--> node[10].face++;
    node[10].face++ <--> { delay = 1ms; }<--> node[11].face++;
    node[8].face++ <--> { delay = 1ms; }<--> node[12].face++;
    node[11].face++ <--> { delay = 1ms; }<--> node[15].face++;
    node[12].face++ <--> { delay = 1ms; }<--> node[13].face++;
    node[13].face++ <--> { delay = 1ms; }<--> node[14].face++;
    node[14].face++ <--> { delay = 1ms; }<--> node[15].face++;
    node[0].face++ <--> { delay = 1ms; }<--> node[12].face++;
    node[1].face++ <--> { delay = 1ms; }<--> node[13].face++;
    node[2].face++ <--> { delay = 1ms; }<--> node[14].face++;
    node[3].face++ <--> { delay = 1ms; }<--> node[15].face++;
    node[0].face++ <--> { delay = 1ms; }<--> node[3].face++;
    node[4].face++ <--> { delay = 1ms; }<--> node[7].face++;
    node[8].face++ <--> { delay = 1ms; }<--> node[11].face++;
    node[12].face++ <--> { delay = 1ms; }<--> node[15].face++;
   

    for i = 0..n-1{
	//node[12].face++ <--> { delay = 1ms; }<--> { delay = 0.01ms; } <--> { delay = 1ms; }<--> client[0].client_port;
	node[i].face++ <-->  { delay = 1ms; } <--> client[i].client_port;
    }
	    

}
