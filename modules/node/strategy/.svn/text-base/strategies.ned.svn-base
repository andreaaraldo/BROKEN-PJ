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
    gates:
	inout strategy_port;
}

simple nearest_repository extends strategy_layer
{
    @class(nearest_repository);
}

simple random_repository extends strategy_layer
{
    @class(random_repository);
}

simple parallel_repository extends strategy_layer
{
    @class(parallel_repository);
}


simple dynamic_learning extends strategy_layer{
    parameters:
	int cut_off = default(1);
    @class(dynamic_learning);
}
