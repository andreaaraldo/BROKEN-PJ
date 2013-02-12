package modules.node;
simple local_statistics{
    parameters:
	@display("i=block/table2;is=l");

	double ts = default(0.1);
	double window = default (60);
	double steady = default(1000);
	double partial_n = default(-1);

	int CEXPL = default(3);
	double ttl = default(30);

}
