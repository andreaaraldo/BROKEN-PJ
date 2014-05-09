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
#ifndef COSTPROBPRODTAILPERF_POLICY_H_
#define COSTPROBPRODTAILPERF_POLICY_H_

//<aa>
#include "decision_policy.h"
#include "error_handling.h"
#include "costprob_policy.h"
#include "WeightedContentDistribution.h"
#include "lru_cache.h"

class Costprobtailperf: public Costprob{
	private:
		double alpha;
		lru_cache* mycache; // cache I'm attached to

    public:
		Costprobtailperf(double average_decision_ratio_, lru_cache* mycache_):
			Costprob(average_decision_ratio_)
		{
			if (xi>1 || xi<0){
				std::stringstream ermsg; 
				ermsg<<"xi="<<xi<<" is not valid";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			alpha = content_distribution_module->get_alpha();
			mycache = mycache_;
		};

		virtual bool data_to_cache(ccn_data * data_msg)
		{

			#ifdef SEVERE_DEBUG
			if( !mycache->is_initialized() ){
				std::stringstream ermsg; 
				ermsg<<"base_cache is not initialized.";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			#endif

			if (! mycache->full() )
				return true;

			chunk_t content_index = data_msg->getChunk();

			double popularity_estimation = 1./pow(content_index, alpha);
			double cost = data_msg->getCost();
			double integral_cost_new = popularity_estimation * cost;

			lru_pos* lru_element_descriptor = mycache->get_lru();
			content_index = lru_element_descriptor->k;
			popularity_estimation = 1./pow(content_index, alpha);
			cost = lru_element_descriptor->cost;
			double integral_cost_lru = cost * popularity_estimation;

			if (integral_cost_new > integral_cost_lru)
				// Inserting this content in the cache would make it better
				return true;

			// a large xi means that we tend to renew the cache often

			//else
			if ( dblrand() < xi )
				return true;
			//else
			return false;
		};

		virtual double compute_correction_factor(){
			return 0;
		};
};
//<//aa>
#endif

