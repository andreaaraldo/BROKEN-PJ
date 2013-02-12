packet ccn_interest{
	@customize(true);
	abstract int path[];

	unsigned long chunk;
	short hops;
	short target = -1;
	short iif=0;
	double btw = 0;
}
