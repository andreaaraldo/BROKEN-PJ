#ifndef NRR_H_
#define NRR_H_
/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Andrea Araldo (developer, mailto andrea.araldo@gmail.com) 
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
#include <omnetpp.h>
#include "MultipathStrategyLayer.h"

class ProbabilisticSplitStrategy: public MultipathStrategyLayer{
    public:
		void initialize();
		bool *get_decision(cMessage *in);
		bool *exploit(ccn_interest *interest);
		int nearest(vector<int>&);
		void finish();
    private:
		unordered_map<name_t,int_f> dynFIB;
		unordered_set<chunk_t> ghost_list;
		vector<Centry> cfib;
		int TTL;
		
		float[] split_factors;

};
#endif
