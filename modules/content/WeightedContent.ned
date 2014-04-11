package modules.content;

//<aa>
simple WeightedContentDistribution extends content_distribution{

    parameters:
		@class(WeightedContentDistribution);
		string weights="0.5 0.5"; //weights of the different repositories
}
//</aa>
