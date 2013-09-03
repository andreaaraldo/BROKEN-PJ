package modules.content;

simple content_distribution {
    parameters:
	double file_size = default (10);
    	int objects = default (100);
	double alpha = default (1);
	double cut_off = default(1);

	@display("i=block/browser;is=l");
	
}
