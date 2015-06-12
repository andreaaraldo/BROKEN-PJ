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
class DecisionPolicy;



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

class base_cache : public abstract_node{
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
		//</aa>
	
    public:
		#ifdef SEVERE_DEBUG
		base_cache():abstract_node(){initialized=false; };
		#endif

		//Outside function behaviour
		int get_size() 
		{
	        std::stringstream ermsg; 
			ermsg<<"In this version of ccnsim, get_size() has been replaced by get_slots()";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );

			return 1; 
		}

		//<aa>
		int get_slots() { return cache_slots; }
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

		virtual bool full() = 0; //<aa> moved from protected to public</aa>

    private:
		//<aa> I replaced cache_size with cache_slots </aa>
		unsigned cache_slots;// <aa> A cache slot is the elementary unit of cache space. A chunk can occupy
						// one or more cache slots, depending on its representation level </aa>

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
};

#endif
