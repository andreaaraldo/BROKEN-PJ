packet ccn_data{
	@customize(true);
	unsigned long chunk;
	short hops = 0;
	int target = -1;
	int iif;
	double btw = 0;
}
