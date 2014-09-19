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
#ifndef COSTAWARE_PARENT_POLICY_H_
#define COSTAWARE_PARENT_POLICY_H_

//<aa>
#include "decision_policy.h"
#include "error_handling.h"
#include "costaware_ancestor_policy.h"
#include "WeightedContentDistribution.h"

// This is an abstract class
class Costaware_parent: public Costaware_ancestor{
    public:
		Costaware_parent(double average_decision_ratio_):
			Costaware_ancestor(average_decision_ratio_){}

		virtual bool data_to_cache(ccn_data * data_msg)
		{
 	
			double x = dblrand();
			double cost = data_msg->getCost();


			if (x < ( (double)  (pow(cost, xi) )* correction_factor) )
					return true;

			return false;
		}

		virtual double compute_correction_factor()= 0; // This is an abstract class
};
//<//aa>
#endif

