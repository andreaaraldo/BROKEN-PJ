package modules.clients;

simple client{
    parameters:
    	@display("i=abstract/people;is=l");
    	double lambda = default(1);
		double W = default(1); //TODO
		double check_time = default(0.1);
		double RTT = default(0.1);
		string type = default("real");
	gates:
    	inout client_port;
}

// This is not a real client; It is a proactive component of a node,
// i.e. a component of a node that decides to send interests in order to
// improve the cache content
simple ProactiveComponent extends client{
	parameters:
		type = "proactive_component";
		lambda = 0; // It does not spontanously send interests
}

