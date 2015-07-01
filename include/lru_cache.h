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

#ifndef LRU_CACHE_H_
#define LRU_CACHE_H_

#include <boost/unordered_map.hpp>
#include "base_cache.h"
#include "ccnsim.h"
#include "error_handling.h"
#include "statistics.h"



using namespace std;
using namespace boost;


//{ RETROCOMPATIBILITY
	struct lru_pos{
		lru_pos() 
		{
			std::stringstream ermsg; 
			ermsg<<"In this version of ccnSim, lru_pos has been replaced by base_cache::cache_item_descriptor. ";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		};
	};
//} RETROCOMPATIBILITY

//Defines a simple lru cache composed by a map and a list of position within the map.
class lru_cache:public base_cache{
    friend class statistics;
    public:
		lru_cache():base_cache(),lru_(NULL),mru_(NULL){}

		bool data_store(ccn_data* data_msg);
		virtual void initialize();
		//<aa>
		cache_item_descriptor* get_mru();
		cache_item_descriptor* get_lru();
		const cache_item_descriptor* get_eviction_candidate();
		bool is_it_empty() const;
		virtual const char* get_cache_content(); // Print the cache content from the mru to the lru
		
		#ifdef SEVERE_DEBUG
		virtual void check_if_correct();
		#endif
		//</aa>
	
		double get_cache_value();	//<aa> It gives an indication of the cost of objects stored 
									// in the cache. </aa>
		double get_average_price();
		

    protected:		
		cache_item_descriptor* data_lookup(chunk_t);// Returns the pointer to the cache item
													//descritor or NULL if no item is found
		bool fake_lookup(chunk_t);
		//<aa>
		void set_mru(cache_item_descriptor* new_mru);
		void set_lru(cache_item_descriptor* new_lru);
		virtual void shrink();
		virtual void remove_from_cache(chunk_t chunk_id, unsigned storage_space); //deprecated
		virtual void insert_into_cache(cache_item_descriptor* descr);
		virtual void remove_from_cache(cache_item_descriptor* descr);
		//</aa>

		void dump();

		cache_item_descriptor* lru_; //least recently used item
		cache_item_descriptor* mru_; //most recently used item

};
#endif
