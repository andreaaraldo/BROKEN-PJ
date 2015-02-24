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


//Indicate the position within the lru cache. In order to look-up for an
//element it just suffices removing the element from the current position and
//inserting it within the head of the list
struct lru_pos{
	lru_pos() : price_(-1) {}; // Initialize the price as undefined

    //older and newer track the lru_position within the 
    //lru cache
    lru_pos* older;
    lru_pos* newer;
    chunk_t k;
    simtime_t hit_time;
	//<aa>
	// double cost;	// now called price
	double price_;   //meaningful only with cost aware caching. In previous versions 
					//of ccnsim it was called cost

	double get_price(){
		#ifdef SEVERE_DEBUG
		if ( statistics::record_cache_value && price_ <0 )
		{
			std::stringstream ermsg; 
			ermsg<<"price is "<< price_ <<", i.e. it is not correctly initialized.";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif

		return price_; 
	}

	void set_price(double new_price) 
	{
		price_ = new_price;
		#ifdef SEVERE_DEBUG
		if ( statistics::record_cache_value && price_ < 0 )
		{
			std::stringstream ermsg; 
			ermsg<<"price=="<< price_ <<", new_price=="<<new_price<<", i.e. it is not initialized.";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif
	}
	//</aa>
};

//Defines a simple lru cache composed by a map and a list of position within the map.
class lru_cache:public base_cache{
    friend class statistics;
    public:
		lru_cache():base_cache(),actual_size(0),lru_(0),mru_(0){;}
		//<aa>
		lru_pos* get_mru();
		lru_pos* get_lru();
		const lru_pos* get_eviction_candidate();
		//</aa>
	
		bool full(); //<aa> moved from protected to public </aa>
		double get_cache_value();	//<aa> It gives an indication of the cost of objects stored 
									// in the cache. </aa>

    protected:
		void data_store(chunk_t);
		bool data_lookup(chunk_t);
		bool fake_lookup(chunk_t);
		//<aa>
		void set_mru(lru_pos* new_mru);
		void set_lru(lru_pos* new_lru);
		//</aa>

		void dump();



    private:
		uint32_t actual_size; //actual size of the cache
		lru_pos* lru_; //least recently used item
		lru_pos* mru_; //most recently used item

		unordered_map<chunk_t, lru_pos*> cache; //cache of values

		virtual void set_price_to_last_inserted_element(double price);

};
#endif
