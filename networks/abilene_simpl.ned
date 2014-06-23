package networks;

network abilene_simpl_network extends base_network{

    parameters:
        //Number of ccn nodes
    	n = 4;

//0->0; 3->1; 5->2; 7-> 3


		num_clients = 4;
		node_clients = "0,1,2,3";
		num_repos = 4;
		node_repos = "0,1,2,3";


connections allowunconnected:
	node[0].face++ <--> { delay = 10.90ms; } <--> node[1].face++;
	node[1].face++ <--> { delay = 4.70ms; } <--> node[2].face++;
	node[2].face++ <--> { delay = 4.01ms; } <--> node[3].face++;
	node[3].face++ <--> { delay = 16.61ms; } <--> node[0].face++;
	node[1].face++ <--> { delay = 2.30ms; } <--> node[3].face++;
}
