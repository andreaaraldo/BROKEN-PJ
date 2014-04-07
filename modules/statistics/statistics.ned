package modules.statistics;
simple statistics{
    parameters:
		double ts = default(0.1);
		double window = default (60);
		double steady = default(1000);
		double partial_n = default(10);

		int CEXPL = default(3);
		double ttl = default(30);
		@display("i=block/table2;is=l");

}
