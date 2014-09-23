package modules.content;

//<aa>
simple WeightedContentDistribution extends content_distribution{

    parameters:
		@class(WeightedContentDistribution);
		string weights; //weights of the different repositories
		bool replication_admitted = default(false);
		double priceratio; // See [icn14]
		double kappa; // costaware exponent. See [icn14]
}
//</aa>


// References
// [icn14] A. Araldo, D. Rossi, F. Martignon, “Design and Evaluation of Cost-aware Information Centric Routers”, to appear in ACM Conference on Information-Centric Networking (ICN), Paris, 2014 
