/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Andrea Araldo (developer, mailto raffaele.chiocchetti@gmail.com)
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
#ifndef ICNCHANNEL_H_
#define ICNCHANNEL_H_

#include <cdataratechannel.h>
#include <ccnsim.h>
#include "statistics.h"

class IcnChannel: public cDatarateChannel {

	public:	
		void clear_stat();
		double get_cost(); // num_objects_served * price;
		double get_price();

	private:
		statistics* statistics_object; // Reference to the statistics object
        
    protected:
        double price;
//        double price_powered; REMOVE THIS LINE
        virtual void initialize();
        virtual void processMessage(cMessage *msg, simtime_t t, result_t& result);

		// Statistics
		long count; //count how many objects have passed through this channel
};

#endif /* ICNCHANNEL_H_ */
//</aa>
