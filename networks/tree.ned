package networks;

network tree_network extends base_network{

    parameters:
        //Number of ccn nodes
    	n = 31;

	//Number of repositories
	node_repos = "0";
	num_repos = 1;
	replicas = 1;

        //Number of clients
	node_clients = "15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30";
	num_clients = 16;



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
}
