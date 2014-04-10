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

/* Fixed probability policy: store a given chunk with a given (fixed)
 * probability. 
 */
class Costprob: public DecisionPolicy{
    public:
		Costprob(double sens_):sens(sens_){;}//Store the caching probability

		virtual bool data_to_cache(ccn_data * data_msg){

			double x = dblrand();
			double cost = data_msg->getCost();

			if (x < cost*sens)
					return true;

			return false;
		}
    private:
		double sens; // sensitivity to price
};
//<//aa>
#endif

