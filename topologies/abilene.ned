package topologies;
import modules.*;

network abilene_network {

    parameters:
    	int n = 11;
	double degree = default(4);

	int c = default(8);

    submodules:
 	statistics: statistics{
	}

	routing: routing{
	}

	request_generator[c]: global_generator {
	        @display("i=block/routing");
	    
	}

    	content_distribution: content_distribution{
	    parameters:
		r =  default(1);
	
	}

        ccn_node[n]: ccn_node{
            parameters:
	        @display("i=block/broadcast_s;is=s");
            //    @display("p=50,50,ri,200,200");
        }


connections allowunconnected:
	ccn_node[1].face++ <--> { delay = 5.48ms; } <--> ccn_node[0].face++;
	ccn_node[10].face++ <--> { delay = 3.80ms; } <--> ccn_node[0].face++;
	ccn_node[10].face++ <--> { delay = 5.02ms; } <--> ccn_node[1].face++;
	ccn_node[10].face++ <--> { delay = 1.68ms; } <--> ccn_node[9].face++;
	ccn_node[2].face++ <--> { delay = 2.99ms; } <--> ccn_node[1].face++;
	ccn_node[3].face++ <--> { delay = 2.43ms; } <--> ccn_node[2].face++;
	ccn_node[4].face++ <--> { delay = 0.88ms; } <--> ccn_node[3].face++;
	ccn_node[5].face++ <--> { delay = 3.82ms; } <--> ccn_node[4].face++;
	ccn_node[6].face++ <--> { delay = 1.10ms; } <--> ccn_node[5].face++;
	ccn_node[7].face++ <--> { delay = 2.30ms; } <--> ccn_node[3].face++;
	ccn_node[7].face++ <--> { delay = 2.91ms; } <--> ccn_node[6].face++;
	ccn_node[8].face++ <--> { delay = 3.48ms; } <--> ccn_node[2].face++;
	ccn_node[8].face++ <--> { delay = 3.77ms; } <--> ccn_node[7].face++;
	ccn_node[9].face++ <--> { delay = 7.36ms; } <--> ccn_node[8].face++;

}
