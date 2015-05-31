package networks;

network video_simple_scenario_network extends base_network{

    parameters:
        //Number of ccn nodes
    	n = 2;

	//Number of repositories
	node_repos = "2";
	num_repos = 1;
	replicas = 1;

	//Number of clients 
	num_clients = 1;
	node_clients = "1";
	
	node[0..2].content_store.C = 0;

connections:
    node[1].face++ <--> {delay = 1ms; datarate = 490Mbps} <--> node[2].face++;
    
}
