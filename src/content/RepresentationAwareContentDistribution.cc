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
#include "RepresentationAwareContentDistribution.h"
#include "content_distribution.h"
#include <error_handling.h>
#include <core_layer.h>

Register_Class(RepresentationAwareContentDistribution);


//<aa>
const unsigned short content_distribution::get_representation_number(chunk_t chunk_id)
{
	#ifdef SEVERE_DEBUG
		ccn_data::check_representation_mask(chunk_id);
	#endif

	unsigned short representation = 0;
	representation_mask_t repr_mask = __representation_mask(chunk_id);
	unsigned short i=1; while (representation == 0 && i<=representation_bitrates_p->size() )
	{
		if( (repr_mask >> i ) == 0 )
			representation = i;
		i++;
	}

	return representation;
}

const unsigned content_distribution::get_storage_space_of_chunk(chunk_t chunk_id) 
{
	#ifdef SEVERE_DEBUG
		ccn_data::check_representation_mask(chunk_id);
	#endif

	return get_storage_space_of_representation(get_representation_number(chunk_id));
}

const double content_distribution::get_bitrate(unsigned short representation)
{
	return (*representation_bitrates_p)[representation-1]; 
}

const unsigned content_distribution::get_storage_space_of_representation(unsigned short representation)
{
	#ifdef SEVERE_DEBUG
	if (representation == 0)
	{
			std::stringstream ermsg; 
			ermsg<<"Representation 0 does not exist";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}	
	#endif

	unsigned space = (*representation_storage_space_p)[representation-1];

	#ifdef SEVERE_DEBUG
	if (space == 0)
	{
			std::stringstream ermsg; 
			ermsg<<"Representation cannot require 0 space";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}	
	#endif
	return space;
}

const unsigned short content_distribution::get_number_of_representations() 
{
	return representation_storage_space_p->size();
}

const representation_mask_t content_distribution::set_bit_to_zero(representation_mask_t mask, unsigned short position)
{
	representation_mask_t adjoint = (0x0001 << (position-1) );
	return ~( (~mask) | adjoint);
	
}

//</aa>
