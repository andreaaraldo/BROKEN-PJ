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
#ifndef COSTPROBPROD_POLICY_H_
#define COSTPROBPROD_POLICY_H_

//<aa>
#include "decision_policy.h"
#include "error_handling.h"
#include "costprob_policy.h"
#include "WeightedContentDistribution.h"

class Costprobtailperf: public Costprob{
    public:
		Costprobprod(double average_decision_ratio_):
			Costprob(average_decision_ratio_)
		{
			if (xi>1 || xi<0){
				std::stringstream ermsg; 
				ermsg<<"xi="<<xi<<" is not valid";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			alpha =
			cout << "vedi che alpha e "<<alpha<<endl;
 		
		}

		virtual bool data_to_cache(ccn_data * data_msg)
		{
 	
			chunk_t content_index = 
			double popularity_estimation = 1./pow(content_index, alpha);
			double cost = msg->getCost();
			double integral_cost_new = popularity_estimation * cost;

			lru_pos* lru_element_descriptor = ;
			content_index = lru_element_descriptor->get...
			popularity_estimation = 1./pow(content_index, alpha);
			cost = lru_element_descriptor->cost;
			double integral_cost_lru = cost * popularity_estimation;

			if (integral_cost_new > integral_cost_lru)
				return true;

			// a small xi means 

			//else
			if ( dblrand() < xi )
				return true;
			//else
			return false;
		}

};
//<//aa>
#endif

