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
#ifndef COSTPROBPRODTAILSMART_POLICY_H_
#define COSTPROBPRODTAILSMART_POLICY_H_

//<aa>
#include "decision_policy.h"
#include "error_handling.h"
#include "costprob_policy.h"
#include "lru_cache.h"
#include "WeightedContentDistribution.h"
#include "costprobtail_policy.h"

class Costprobtailsmart: public Costprobtail{   
	public:
		Costprobtailsmart(double average_decision_ratio_, base_cache* mycache_par):
			Costprobtail(average_decision_ratio_, mycache_par)
		{
			// CHECK_INPUT{
			if (average_decision_ratio_ < 1e-15){
				std::stringstream ermsg; 
				ermsg<<"average_decision_ratio "<<average_decision_ratio_<<" is too small";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			// }CHECK_INPUT
		};

		virtual bool data_to_cache(ccn_data * data_msg)
		{
			bool decision;

			#ifdef SEVERE_DEBUG
			if( !mycache->is_initialized() ){
				std::stringstream ermsg; 
				ermsg<<"base_cache is not initialized.";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			#endif

			// Small xi means cost unawareness

			if ( dblrand() < xi ){
				//cost aware
				if( !mycache->full() )
					decision = true;
				else{
					double new_content_cost = data_msg->getCost();

					lru_pos* lru_element_descriptor = mycache->get_lru();
					double lru_cost = lru_element_descriptor->cost;

					if (new_content_cost >= lru_cost)
						decision = true;
					else
						decision = false;

				}
			}else{
				//cost unaware
				if ( dblrand() <= average_decision_ratio )
					decision = true;
				else
					decision = false;
			}

			if (decision == true)
				set_last_accepted_content_cost(data_msg );

			return decision;
		};


		virtual double compute_content_weight(chunk_t id, double cost)
		{
			std::stringstream ermsg; 
			ermsg<<"You cannot call this method.";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			return 0;
		}


		virtual bool decide_with_cache_not_full(chunk_t id, double cost){
			return true;
		}
};
//<//aa>
#endif

