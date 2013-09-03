package modules.node.strategy;

moduleinterface strategy
{
    gates:
    	inout strategy_port;
}

simple strategy_layer like strategy
{
    parameters:
    	@display("i=block/buffer2;is=l");
	string routing_file=default("");
    gates:
	inout strategy_port;
}

simple spr extends strategy_layer
{
    @class(spr);
}

simple random_repository extends strategy_layer
{
    @class(random_repository);
}

simple parallel_repository extends strategy_layer
{
    @class(parallel_repository);
}


simple nrr1 extends strategy_layer{
    parameters:
	int cut_off = default(1);
	int TTL1 = default(1000);
    @class(nrr1);
}

simple nrr extends strategy_layer{
    parameters:
	int TTL2 = default(1000);
    @class(nrr);
}

