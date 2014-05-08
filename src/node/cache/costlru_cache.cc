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

#include <iostream>
#include "costlru_cache.h"
#include "costprob_policy.h"
#include "error_handling.h"

Register_Class(costlru_cache);


//<aa>
const lru_pos* costlru_cache::get_eviction_candidate(){
	if ( full() ) 
		return get_lru();
	else return NULL;
}

void costlru_cache::data_store(chunk_t elem){
	lru_cache::data_store(elem);
	double cost_of_the_last_accepted_element = 
			( (Costprob*) get_decisor() )->get_last_accepted_content_cost();

	#ifdef SEVERE_DEBUG
	if (cost_of_the_last_accepted_element<0 ){
	    std::stringstream ermsg; 
		ermsg<<"cost_of_the_last_accepted_element="<<cost_of_the_last_accepted_element<<
			", while it MUST NOT be a negative number. Something goes wrong with the "<<
			"initialization of this attribute";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );

	}
	#endif

	
	// Annotate the cost of the last inserted element
	get_mru()->cost = cost_of_the_last_accepted_element;
}
//</aa>
