package networks;

network ndntestbed_network extends base_network{

    parameters:
        //Number of ccn nodes
    	n = 17;
	//node[2].RS = "lru_cache";
	//node[3].RS = "fifo_cache";
	//node[4].RS = "lfu_cache";
	//node[5].RS = "two_cache";
	//node[6].RS = "random_cache";

    connections allowunconnected:
	node[2].face++ <--> { delay = 5.48ms; } <--> node[0].face++;
	node[4].face++ <--> { delay = 5.48ms; } <--> node[3].face++;
	node[4].face++ <--> { delay = 5.48ms; } <--> node[2].face++;
	node[5].face++ <--> { delay = 5.48ms; } <--> node[2].face++;
	node[5].face++ <--> { delay = 5.48ms; } <--> node[4].face++;
	node[6].face++ <--> { delay = 5.48ms; } <--> node[2].face++;
	node[7].face++ <--> { delay = 5.48ms; } <--> node[6].face++;
	node[7].face++ <--> { delay = 5.48ms; } <--> node[1].face++;
	node[7].face++ <--> { delay = 5.48ms; } <--> node[2].face++;
	node[8].face++ <--> { delay = 5.48ms; } <--> node[5].face++;
	node[8].face++ <--> { delay = 5.48ms; } <--> node[2].face++;
	node[8].face++ <--> { delay = 5.48ms; } <--> node[7].face++;
	node[9].face++ <--> { delay = 5.48ms; } <--> node[6].face++;
	node[10].face++ <--> { delay = 5.48ms; } <--> node[7].face++;
	node[10].face++ <--> { delay = 5.48ms; } <--> node[9].face++;
	node[11].face++ <--> { delay = 5.48ms; } <--> node[8].face++;
	node[11].face++ <--> { delay = 5.48ms; } <--> node[10].face++;
	node[11].face++ <--> { delay = 5.48ms; } <--> node[7].face++;
	node[12].face++ <--> { delay = 5.48ms; } <--> node[8].face++;
	node[12].face++ <--> { delay = 5.48ms; } <--> node[9].face++;
	node[12].face++ <--> { delay = 5.48ms; } <--> node[6].face++;
	node[13].face++ <--> { delay = 5.48ms; } <--> node[1].face++;
	node[13].face++ <--> { delay = 5.48ms; } <--> node[9].face++;
	node[13].face++ <--> { delay = 5.48ms; } <--> node[10].face++;
	node[14].face++ <--> { delay = 5.48ms; } <--> node[9].face++;
	node[14].face++ <--> { delay = 5.48ms; } <--> node[11].face++;
	node[14].face++ <--> { delay = 5.48ms; } <--> node[12].face++;
	node[15].face++ <--> { delay = 5.48ms; } <--> node[9].face++;
	node[15].face++ <--> { delay = 5.48ms; } <--> node[14].face++;
	node[16].face++ <--> { delay = 5.48ms; } <--> node[13].face++;
	node[16].face++ <--> { delay = 5.48ms; } <--> node[11].face++;
	node[16].face++ <--> { delay = 5.48ms; } <--> node[15].face++;
}
