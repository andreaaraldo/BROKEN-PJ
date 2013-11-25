package networks; 

import modules.node.BorderNode;
import modules.node.InternalNode;
import modules.node.Inode;

network as_network extends base_network{
    parameters: 
		n = 2; //Number of ccn nodes 
		num_clients = 2;
		node[0].typename="BorderNode";
		node[1].typename="InternalNode";
	
	submodules:
        nodescemo [n]: <default("node")> like Inode; //aa

    connections allowunconnected: 
		node[0].face++ <--> { delay = 5.48ms; } <--> node[1].face++;
		nodescemo[0].face++ <--> { delay = 5.48ms; } <--> nodescemo[1].face++;  

}
