package modules.content;

simple content_distribution like IContentDistribution{
    parameters:
		double file_size = default (10);
		int objects = default (100);
		double alpha = default (1);
		double q = default(0);
		double cut_off = default(1);
		string representation_bitrates = default("1"); // in Mbps

	@display("i=block/browser;is=l");
	
}
