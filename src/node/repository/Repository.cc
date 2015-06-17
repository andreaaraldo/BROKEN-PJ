/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Andrea Araldo (andrea.araldo@gmail.com)
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
#include "error_handling.h"
#include "repository/Repository.h"
#include "statistics.h"
#include <iostream>

//Register_Class(lru_cache);

Repository::Repository(int attached_node_index_, int repo_index_, double price_)
{
	attached_node_index = attached_node_index_;
	repo_index = repo_index_;
	price = price_;
	bitmask = 0; 
	clear_stat();

	#ifdef SEVERE_DEBUG
	if( (unsigned)repo_index > sizeof(bitmask) )
	{
		std::stringstream msg; 
		msg<<"Trying to consider the "<<repo_index_<< "-th repository, while the maximum number can only be "<<
			sizeof(bitmask);
		severe_error(__FILE__, __LINE__, msg.str().c_str() );
	}
	#endif
    bitmask = (1<<repo_index);	// Recall that the width of the repository bitset is only num_repos
}



double Repository::get_price() const
{
	return price;
}

/**
 * Returns a representation of the requested chunk or 0 if the object is not owned by the 
 * repository
*/
unsigned short Repository::handle_interest(ccn_interest* int_msg)
{

	unsigned short representation = 0;
	if (bitmask & __repo( int_msg->get_object_id() ) )
	{
		// This repo contains the object
		repo_load++;

		// By default representation 1 is selected. Extend this class to change this
		representation_mask_t available = 0xFFFF; 	// By definition, a repository contains 
													//all the representations of the served objects
		representation = representation_selector.select(
			int_msg->get_representation_mask(), available);
	}
	return representation;
}

//	Print node statistics
void Repository::finish( cComponent* parentModule) const
{
    char name [30];

    if (repo_load != 0)
	{
		sprintf ( name, "repo_load[%d]", attached_node_index);
		parentModule->recordScalar(name,repo_load);
    }
}

void Repository::clear_stat()
{
	repo_load = 0;
}

int Repository::get_repo_load() const
{
	return repo_load;
}
//</aa>

