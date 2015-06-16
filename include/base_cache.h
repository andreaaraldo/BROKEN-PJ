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
#ifndef B_CACHE_H_
#define B_CACHE_H_


#include "ccnsim.h"
//<aa>
#include "error_handling.h"
#include "content_distribution.h"
#include "statistics.h"
//</aa>
class DecisionPolicy;

// Indicate the position within the cache. It is useful with policies like lru. You can ignore it when you
// you use policies that do not require any ordering.
// In order to look-up for an
// element it just suffices removing the element from the current position and
// inserting it within the head of the list
struct cache_item_descriptor
{
	cache_item_descriptor() : price_(-1) {}; // Initialize the price as undefined

    //older and newer track the lru_position within the 
    //lru cache
    cache_item_descriptor* older;
    cache_item_descriptor* newer;
    chunk_t k; //identifier of the chunk, i.e. [object_id, chunk_number, representation_mask]
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




//Base cache class: it implements the basic behaviour of every cache by the mean of two abstract functions:
//
//-) data_store: stores chunks within the cache with a given policy
//-) data_lookup: return if the given chunk exists within the cache
//
struct cache_stat_entry{
    unsigned int  miss; //Total number of misses
    unsigned int  hit; //Totatle number of hit
    cache_stat_entry():miss(0),hit(0){;}
    double rate(){ return hit *1./(hit+miss);} //return the miss rate of the class
};

class base_cache : public abstract_node
{
    friend class statistics;

    protected:
		void initialize();
		void handleMessage (cMessage *){;}
		void finish();

		//Inteface function (depending by internal data structures of each cache)
		virtual void data_store (chunk_t) = 0; 
		virtual bool data_lookup(chunk_t) = 0;
		virtual void dump(){cout<<"Method dump() not implemented in all subclasses of base_cache. Check that you are using a subclass that implements it."<<endl;}

		//<aa>
		#ifdef SEVERE_DEBUG
		bool initialized;
		#endif

		virtual void insert_into_cache(chunk_t chunk_id_without_representation_mask, 
					cache_item_descriptor* descr, unsigned storage_space);
		virtual void remove_from_cache(chunk_t chunk_id_without_representation_mask,
					unsigned storage_space);
		virtual const uint32_t get_occupied_slots();
		virtual const void update_occupied_slots(int difference);
		virtual unordered_map<chunk_t,cache_item_descriptor *>::iterator find_in_cache(
					chunk_t chunk_id);
		virtual unordered_map<chunk_t,cache_item_descriptor *>::iterator end_of_cache();
		//</aa>
	
    public:
		#ifdef SEVERE_DEBUG
		base_cache():abstract_node(){initialized=false; occupied_slots=0;};
		#endif

		//Outside function behaviour
		int get_size() 
		{
			#ifdef SEVERE_DEBUG
			if( content_distribution::get_number_of_representations() != 1 )
			{
				std::stringstream ermsg; 
				ermsg<<"This function cannot be used if more than one representation per content is considered"<<
					". Use get_slots() directly in all the other cases.";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
			#endif
			return get_slots(); 
		}

		//<aa>
		unsigned  get_slots() { return cache_slots; }
		void set_slots(unsigned);
		//</aa>

		void set_size(uint32_t);

		virtual bool fake_lookup(chunk_t);
		bool lookup(chunk_t);
		// Lookup without hit/miss statistics (used with the 2-LRU meta-caching strategy to lookup the name cache)
		bool lookup_name(chunk_t);
		void store (cMessage *);
		void store_name(chunk_t);    // Store the content ID inside the name cache (only with 2-LRU meta-caching).

		void clear_stat();

		//<aa>
		virtual uint32_t get_decision_yes();
		virtual uint32_t get_decision_no();
		virtual void set_decision_yes(uint32_t n);
		virtual void set_decision_no(uint32_t n);
		virtual const DecisionPolicy* get_decisor();
		virtual void after_discarding_data(); // Call it when you decide not to store an incoming data pkt

		virtual double get_cache_value(){
			cout<<"Method get_cache_value() not implemented in all subclasses of base_cache. Check that you are using a subclass that implements it."<<endl;
			exit(-1);
		}
		virtual double get_average_price(){
			cout<<"Method get_average_price() not implemented in all subclasses of base_cache. Check that you are using a subclass that implements it."<<endl;
			exit(-1);
		}



		virtual void set_price_to_last_inserted_element(double price){
			cout<<"Method set_price_to_last_inserted_element not implemented in all subclasses of base_cache. Check that you are using a subclass that implements it."<<endl;
			exit(-1);
		}

		#ifdef SEVERE_DEBUG
		virtual bool is_initialized();
		#endif
		//</aa>

    private:
		//<aa> I replaced cache_size with cache_slots </aa>
		unsigned cache_slots;// <aa> A cache slot is the elementary unit of cache space. A chunk can occupy
						// one or more cache slots, depending on its representation level </aa>

		int name_cache_size;   		// Size of the name cache expressed in number of content IDs (only with 2-LRU meta-caching).

		int nodes;
		int level;

		DecisionPolicy *decisor;

		//Average statistics
		uint32_t miss;
		uint32_t hit;

		//<aa>
		uint32_t decision_yes;
		uint32_t decision_no;
		//</aa>


		//Per file statistics
		cache_stat_entry *cache_stats;

		uint32_t occupied_slots; //actual size of the cache
		unordered_map<chunk_t, cache_item_descriptor*> cache;
};

#endif
