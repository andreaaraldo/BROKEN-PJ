package modules.node;
import modules.node.strategy.*;
import modules.node.cache.*;

module node {
    parameters:
	string CS = default("random_cache");
	string SL = default("nearest_repository");
	int level = default (-1);
	double betweenness = default (-1);
	@display("i=abstract/router;is=s");
    gates:
	inout face[];	
	inout client_port;

    submodules:
	core_layer : core_layer;
	strategy_layer : < "modules.node.strategy." + SL > like strategy;
	content_store : < "modules.node.cache." + CS > like cache;

    connections :
	for i=0..sizeof(face)-1{
	    core_layer.face++ <--> face [i];
	}

	core_layer.client_port <--> client_port;
	core_layer.cache_port  <--> content_store.cache_port;
	core_layer.strategy_port <--> strategy_layer.strategy_port;
}

