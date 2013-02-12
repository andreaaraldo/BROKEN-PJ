package topologies;
import modules.content.content_distribution;
import modules.statistics.statistics;
import modules.clients.client;
import modules.node.node;

network tree_network {

    parameters:
        //Number of ccn nodes
    	int n = 31;
	int c = 16;

	//Number of repositories
	string node_repos = "0";
	int num_repos = 1;
	int replicas = 1;

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
	client[c]: client;



connections allowunconnected:


//Tree core
node[0].face++ <--> { delay = 1ms; } <-->node[1].face++;
node[0].face++ <--> { delay = 1ms; } <-->node[2].face++;
node[1].face++ <--> { delay = 1ms; } <-->node[3].face++;
node[1].face++ <--> { delay = 1ms; } <-->node[4].face++;
node[2].face++ <--> { delay = 1ms; } <-->node[5].face++;
node[2].face++ <--> { delay = 1ms; } <-->node[6].face++;
node[3].face++ <--> { delay = 1ms; } <-->node[7].face++;
node[3].face++ <--> { delay = 1ms; } <-->node[8].face++;
node[4].face++ <--> { delay = 1ms; } <-->node[9].face++;
node[4].face++ <--> { delay = 1ms; } <-->node[10].face++;
node[5].face++ <--> { delay = 1ms; } <-->node[11].face++;
node[5].face++ <--> { delay = 1ms; } <-->node[12].face++;
node[6].face++ <--> { delay = 1ms; } <-->node[13].face++;
node[6].face++ <--> { delay = 1ms; } <-->node[14].face++;
node[7].face++ <--> { delay = 1ms; } <-->node[15].face++;
node[7].face++ <--> { delay = 1ms; } <-->node[16].face++;
node[8].face++ <--> { delay = 1ms; } <-->node[17].face++;
node[8].face++ <--> { delay = 1ms; } <-->node[18].face++;
node[9].face++ <--> { delay = 1ms; } <-->node[19].face++;
node[9].face++ <--> { delay = 1ms; } <-->node[20].face++;
node[10].face++ <--> { delay = 1ms; } <-->node[21].face++;
node[10].face++ <--> { delay = 1ms; } <-->node[22].face++;
node[11].face++ <--> { delay = 1ms; } <-->node[23].face++;
node[11].face++ <--> { delay = 1ms; } <-->node[24].face++;
node[12].face++ <--> { delay = 1ms; } <-->node[25].face++;
node[12].face++ <--> { delay = 1ms; } <-->node[26].face++;
node[13].face++ <--> { delay = 1ms; } <-->node[27].face++;
node[13].face++ <--> { delay = 1ms; } <-->node[28].face++;
node[14].face++ <--> { delay = 1ms; } <-->node[29].face++;
node[14].face++ <--> { delay = 1ms; } <-->node[30].face++;

//Clients at leaves
node[15].face++<--> { delay = 1ms; } <-->client[0].client_port;
node[16].face++<--> { delay = 1ms; } <-->client[1].client_port;
node[17].face++<--> { delay = 1ms; } <-->client[2].client_port;
node[18].face++<--> { delay = 1ms; } <-->client[3].client_port;
node[19].face++<--> { delay = 1ms; } <-->client[4].client_port;
node[20].face++<--> { delay = 1ms; } <-->client[5].client_port;
node[21].face++<--> { delay = 1ms; } <-->client[6].client_port;
node[22].face++<--> { delay = 1ms; } <-->client[7].client_port;
node[23].face++<--> { delay = 1ms; } <-->client[8].client_port;
node[24].face++<--> { delay = 1ms; } <-->client[9].client_port;
node[25].face++<--> { delay = 1ms; } <-->client[10].client_port;
node[26].face++<--> { delay = 1ms; } <-->client[11].client_port;
node[27].face++<--> { delay = 1ms; } <-->client[12].client_port;
node[28].face++<--> { delay = 1ms; } <-->client[13].client_port;
node[29].face++<--> { delay = 1ms; } <-->client[14].client_port;
node[30].face++<--> { delay = 1ms; } <-->client[15].client_port;
}
