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
#include "ccn_interest_m.h"
#include "base_cache.h"
#include "error_handling.h"
//<aa>

//Register_Class(ProbabilisticSplitStrategy);


void ProbabilisticSplitStrategy::initialize()
{
    MultipathStrategyLayer::initialize();
	// ref: omnet 4.3 manual, sec 4.5.4
	const char *vstr = par("split_factors").stringValue(); // e.g. "aa bb cc";
	split_factors = cStringTokenizer(vstr).asDoubleVector();

	// Verify if split factors are correct
	int out_gates = getParentModule()->gateSize("face$o");
	if (split_factors.size() != out_gates )
		severe_error(__FILE__,__LINE__, "The number of slipt factors is different from the number of output gates");
	double sum = 0;
	for (int i=0; i < split_factors.size() ; i++ )
		sum += split_factors[i];
	if (sum != 1)
		severe_error(__FILE__,__LINE__, "The sum of slipt factors should be 1");
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
	int out_gate = UNDEFINED_VALUE;

	if(FIB_entries.size() == 1)
		out_gate = FIB_entries[0].id;
	else{
		while (out_gate == UNDEFINED_VALUE) 
		{	//extract an out_gate until a valid one is found
			double r = uniform(0, 1);
			double sum = 0; int i = 0;
			while (sum <= r)
			{
				sum += split_factors[i];
				i++;
				#ifdef SEVERE_DEBUG
				if (i >= split_factors.size() || sum > 1)
					severe_error(__FILE__,__LINE__, "error");
				#endif
			}
			int chosen_gate = i;
			// If the chosen gate is included in the FIB_entries,
			// use it. Otherwise, start again the while loop
			for (int j=0; j < FIB_entries.size(); j++){
				int_f entry = FIB_entries[j];
				if (entry.id == chosen_gate){
					out_gate = chosen_gate; break;
				}
			}
		}
	}
		
	return out_gate;
}


int ProbabilisticSplitStrategy::decide_target_repository(ccn_interest *interest)
{
	int repository;
	if (interest->getRep_target() == UNDEFINED_VALUE)
    {
    	// Get all the repositories that store the content demanded by the
    	// interest
		vector<int> repos = interest->get_repos();
		
		// Choose one of them
		repository = repos[intrand(repos.size())];
		interest->setRep_target(repository);
    }else 
		repository = interest->getRep_target();
	
	return repository;
}


bool* ProbabilisticSplitStrategy::exploit(ccn_interest *interest)
{
    int repository,
	outif,
	gsize;

    gsize = __get_outer_interfaces(); //number of gates
	repository = decide_target_repository(interest);
	severe_error(__FILE__,__LINE__,"Enable the above 2 lines and return value must be bool*");

    bool *decision = new bool[gsize];
	std::fill(decision,decision+gsize,0);
    	
	const vector<int_f> FIB_entries = get_FIB_entries(repository);
	int s = FIB_entries.size();
	int i = 0;
	while (i < s)
	{
	    outif = FIB_entries[i].id;
	    decision[outif]=true;
		i++;
	}

    return decision;
}

void ProbabilisticSplitStrategy::finish(){
    MultipathStrategyLayer::finish();
}
//</aa>
