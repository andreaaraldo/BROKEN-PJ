/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *    Andrea Araldo (mailto andrea.araldo@gmail.com)
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
#ifndef COSTPROB_POLICY_H_
#define COSTPROB_POLICY_H_

//<aa>
#include "decision_policy.h"
#include "error_handling.h"
#include "WeightedContentDistribution.h"

class Costprob: public DecisionPolicy{
    protected:
		double average_decision_ratio;
		double correction_factor; // An object will be cached with prob correction_factor * cost
		double xi;
		double priceratio;
		vector<double> weights;
		double last_accepted_content_cost;

    public:
		Costprob(double average_decision_ratio_)
		{
			last_accepted_content_cost=-1;
			average_decision_ratio = average_decision_ratio_;

		    vector<string> ctype;
			ctype.push_back("modules.content.WeightedContentDistribution");
			cTopology topo;
	   		topo.extractByNedTypeName(ctype);
			int num_content_distribution_modules = topo.getNumNodes();


			#ifdef SEVERE_DEBUG
				if (num_content_distribution_modules != 1){
					std::stringstream msg; 
					msg<<"Found "<< num_content_distribution_modules << ". It MUST be 1";
					severe_error(__FILE__, __LINE__, msg.str().c_str() );
				}
			#endif

			cTopology::Node *content_distribution_node = topo.getNode(0);
			WeightedContentDistribution* content_distribution_module = 
					(WeightedContentDistribution*) content_distribution_node->getModule();

			#ifdef SEVERE_DEBUG
			if ( !content_distribution_module->isInitialized() ){
					std::stringstream msg; 
					msg<<"content_distribution_module is not initialized";
					severe_error(__FILE__, __LINE__, msg.str().c_str() );
			}
			#endif

			weights = content_distribution_module->get_weights();
			unsigned num_repos = weights.size();
			priceratio = content_distribution_module->get_priceratio();
			xi = content_distribution_module->get_xi();
			
			{ //check
				if (num_repos != 3){
					std::stringstream msg; 
					msg<<"num_repos = "<< num_repos << ". But only 3 is admitted";
					severe_error(__FILE__, __LINE__, msg.str().c_str() );
				}
			}
		}


		virtual bool data_to_cache(ccn_data * data_msg) = 0;

		virtual void finish (int nodeIndex, base_cache* cache_p){			
		    char name [30];
			sprintf ( name, "correction_factor[%d]", nodeIndex);
			cache_p->recordScalar (name, correction_factor);

			sprintf ( name, "xi[%d]", nodeIndex);
			cache_p->recordScalar (name, xi);
		};

		virtual double get_last_accepted_content_cost(){
			return last_accepted_content_cost;
		};

		virtual void set_last_accepted_content_cost(ccn_data * data_msg){
			last_accepted_content_cost = data_msg->getCost();
		}

		virtual double compute_correction_factor()=0;
};
//<//aa>
#endif

