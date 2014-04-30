package modules.content;

//<aa>
simple WeightedContentDistribution extends content_distribution{

    parameters:
		@class(WeightedContentDistribution);
		string weights; //weights of the different repositories
		bool replication_admitted;
		double priceratio;
}
//</aa>
