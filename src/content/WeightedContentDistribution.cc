/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
//<aa>

#include "ccnsim.h"
#include "WeightedContentDistribution.h"
#include <error_handling.h>

Register_Class(WeightedContentDistribution);

void WeightedContentDistribution::initialize(){
	const char *str = par("weights").stringValue();
	weights = cStringTokenizer(str).asDoubleVector();

	// Input consistency check
	{
        std::stringstream ermsg; 

		if ( weights.size() != 2 ) {
			ermsg<<"For the time being it works only with 2 repos";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}

		double sum = 0;
		for (unsigned i=0; i < weights.size(); i++)
		{
			if ( weights[i] > 1 ||  weights[i] < 0){
				ermsg<<"Weight : "<<weights[i]<<" is invalid";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			sum += weights[i];
		}
		if ( sum != 1 ){
			ermsg<<"Sum of weights : "<<sum <<" is invalid";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	}

	content_distribution::initialize();
}

vector<int> WeightedContentDistribution::binary_strings(int num_ones,int len){
	// Do nothing as we don't need this in this case
	vector<int> v;
	return v;
}

// The repository with bigger weight will have the more contents
//		PAY ATTENTION: 
//			- Verify the correctness of catalog_weights before calling
//				this method. Their sum must be 1 and 
//			- For the time being, it works only with 2 repositories with
//			-	1 replica per object
int WeightedContentDistribution::choose_repos ( ){
	double rand_num = dblrand();
	int repo_string = (rand_num < weights[0]) ? 1 : 3 ;
	// Remember that 1 is the binary string 0...01 and it corresponds to the
	// object placed only in the 1st repository. 3 is the string 0...10 and
	// it corresponds to the object placed only in the 2ns repo.

	std::stringstream ermsg; 
	return repo_string;
}
