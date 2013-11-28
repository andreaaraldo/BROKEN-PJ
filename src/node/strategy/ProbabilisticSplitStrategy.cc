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
#include <omnetpp.h>
#include "ProbabilisticSplitStrategy.h"
#include "ccnsim.h"
#include "ccn_interest.h"
#include "base_cache.h"
//<aa>
Register_Class(ProbabilisticSplitStrategy);


void ProbabilisticSplitStrategy::initialize()
{
    MultipathStrategyLayer::initialize();
}

bool* ProbabilisticSplitStrategy::get_decision(cMessage *in){

    bool *decision;
    if (in->getKind() == CCN_I){
	ccn_interest *interest = (ccn_interest *)in;
	decision = exploit(interest);
    }
    return decision;
}

int ProbabilisticSplitStrategy::decide_out_gate(vector<int_f> FIB_entries)
{
	int undecided = -1;
	int out_gate = undecided;
	if(FIB_entries.size() == 1)
		out_gate = FIB_entries[0].id;
	else{
		while (out_gate == undecided){
			ciao
		}
	}
		
	return out_gate;
}

int ProbabilisticSplitStrategy::decide_target_repository(ccn_interest *interest)
{
   if (interest->getRep_target == ccn_interest_Base.UNDEFINED_VALUE)
    {
    	//<aa> Get all the repositories that store the content demanded by the
    	// interest </aa>
		vector<int> repos = interest->get_repos();
		
		//<aa> Choose one of them </aa>
		repository = random(repos);
		interest->setRep_target(repository);
    }else 
		repository = interest->getRep_target();
	
	return repositoiry
}

bool* ProbabilisticSplitStrategy::exploit(ccn_interest *interest)
{
    int repository,
	outif,
	gsize;

    gsize = __get_outer_interfaces(); //number of gates
 	repository = decide_target_repository(ccn_interest *interest);
	const vecotr<int_f> FIB_entries = get_FIB_entry(repository);

    outif = FIB_entry.id;
    bool *decision = new bool[gsize];
    std::fill(decision,decision+gsize,0);
    decision[outif]=true;

    return decision;
}


void nrr::finish(){
    MultipathStrategyLayer::finish();
}
//</aa>
