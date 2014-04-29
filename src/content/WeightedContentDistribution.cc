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
//<aa>

#include "ccnsim.h"
#include "WeightedContentDistribution.h"
#include "IcnChannel.h"
#include <error_handling.h>

Register_Class(WeightedContentDistribution);

void WeightedContentDistribution::initialize(){
	const char *str = par("weights").stringValue();
	weights = cStringTokenizer(str).asDoubleVector();
	replication_admitted = par("replication_admitted");

	std::stringstream ermsg; 

	// Input consistency check
	{

		double sum = 0;
		for (unsigned i=0; i < weights.size(); i++)
		{
			if ( weights[i] > 1 ||  weights[i] < 0){
				ermsg<<"Weight : "<<weights[i]<<" is invalid";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			sum += weights[i];
		}
		if ( sum != 1 ){
			ermsg<<"Sum of weights : "<<sum <<" is invalid";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	}

	content_distribution::initialize();

	// Other checks
	{
		if ( degree != -1 ){
			ermsg<<"degree is "<<degree <<". But with this content distribution model "<<
				"this value is ignored. Please, set degree = -1";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}

		#ifdef SEVERE_DEBUG
		verify_prices();
		if (!replication_admitted && total_replicas != cardF)
		{
	        std::stringstream ermsg; 
			ermsg<<"total_replicas="<<total_replicas<<"; cardF="<<cardF<<". ";
			ermsg<<"Since replication_admitted==false, There MUST be 1 replica for each content ";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif
	}
}

#ifdef SEVERE_DEBUG
/**
 * from_repo_to_channel[repo_idx]   is the pointer to the IcnChannel through which you can 
 *									reach the repository repo_idx
 */
void WeightedContentDistribution::verify_prices()
{
	unsigned num_repos = weights.size();
	unsigned previous_repo_price = 0;

	for (unsigned repo_idx=0; repo_idx < num_repos; repo_idx++)
	{
		// get the node associated to the repository
		int node_idx = repositories[repo_idx];
		// The repository is attached to node[node_idx]

	    vector<string> ctype;
	    ctype.push_back("modules.node.node");
		cTopology topo;
   		topo.extractByNedTypeName(ctype);
		cTopology::Node *node = topo.getNode(node_idx);

		if ( node->getModule()->gateSize("face$o") != 2)
		{
	        std::stringstream ermsg; 
			ermsg<<"Found "<< node->getModule()->gateSize("face$o")<<
				" ports."<<
				"The nodes attached to a repo admit only 2 output ports: port 0 attached "
				<<" to the client and port 1 attached the next node toward the clients";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		cGate *gate = node->getModule()->gate("face$o", 1);
		IcnChannel *ch = (IcnChannel*) gate->getChannel();
		double repo_price = ch->get_price();
		if (repo_price < previous_repo_price)
		{
	        std::stringstream ermsg; 
			ermsg<<"Prices must be non descending. This is due to the way choose_repo() works.";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		previous_repo_price = repo_price;
	}
}
#endif

#ifdef SEVERE_DEBUG
// Override content_distribution::verify_replica_number()
void WeightedContentDistribution::verify_replica_number(){
	// Do nothing
}
#endif

// Override content_distribution::finalize_total_replica()
void WeightedContentDistribution::finalize_total_replica(){
	// Do nothing
}



// Override content_distribution::binary_strings(..)
vector<int> WeightedContentDistribution::binary_strings(int num_ones,int len){
	// Do nothing as we don't need this in this case
	vector<int> v;
	return v;
}

// The repository with bigger weight will have the more contents
//		PAY ATTENTION: 
//			- Verify the correctness of catalog_weights before calling
//				this method. Their sum must be 1 and 
int WeightedContentDistribution::choose_repos ( )
{
	unsigned num_repos = weights.size();
	int assigned_repo = -1;

	if (replication_admitted) 
	{
		for (unsigned repo_idx = 0; repo_idx < num_repos; repo_idx++)
		{
			if (dblrand() < weights[repo_idx] ){
				total_replicas++;
				if ( assigned_repo==-1 )
					// The object has not been assigned yet. Assign it to repo_idx
					assigned_repo = repo_idx;

				// else: do nothing
					// The object has already been assigned to a previous repository.
					// Since the previous repositories are cheaper than the current
					// repo_idx, the copy on the current repository will be ignored.
					// It is like it does not exist.
			}
		}
	} 	// else: leave the object unassigned.
			// If replication is not admitted, we assign the object to one and only one repository.
			// To do so, we leverage the following code

	if ( assigned_repo == -1 )
	{
		// The object has not been assigned yet. We have to force it in some
		// repository
		double rand_num = dblrand();
		double accumulated_weight = 0;
		assigned_repo = 0;
		while (rand_num > accumulated_weight){
			accumulated_weight += weights[ assigned_repo ];
			assigned_repo++;
		}
		assigned_repo--;
		total_replicas++;
	}
	// The object will be assigned to the repo_idx-th repository.
	// Set the repo_idx-th bit in the binary string
	int repo_string = 0;
	repo_string |= 1 << assigned_repo; //http://stackoverflow.com/a/47990

	#ifdef SEVERE_DEBUG
		int num_1_bits =  __builtin_popcount (repo_string); //http://stackoverflow.com/a/109069
												// Number of bits set to 1 (corresponding 
												// to the number of repositories this object was 
												// assigned to)
		if ( assigned_repo > (int) num_repos-1 || assigned_repo < 0){
		    std::stringstream ermsg; 
			ermsg<<"assigned_repo="<<assigned_repo;
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		if (num_1_bits != 1 && !replication_admitted){
		    std::stringstream ermsg; 
			ermsg<<"repo_string="<<repo_string;
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif

	return repo_string;
}
