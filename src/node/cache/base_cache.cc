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
#include <cmath>
#include "base_cache.h"
#include "core_layer.h"
#include "statistics.h"
#include "content_distribution.h"
#include "ccn_data_m.h"

#include "fix_policy.h"
//<aa>
#include "ideal_blind_policy.h"
#include "costaware_policy.h"
#include "ideal_costaware_policy.h"
#include "error_handling.h"
#include "ccn_data.h"
//</aa>
#include "two_lru_policy.h"
#include "lcd_policy.h"
#include "never_policy.h"
#include "always_policy.h"
#include "decision_policy.h"
#include "betweenness_centrality.h"
#include "prob_cache.h"

#include "ccnsim.h"


//Initialization function
void base_cache::initialize()
{
    nodes      = getAncestorPar("n");
    level = getAncestorPar("level");

	initialize_cache_slots();

    //{ INITIALIZE DECISION POLICY
	decisor = NULL;
    string decision_policy = par("DS");
	double target_acceptance_ratio;
	string target_acceptance_ratio_string;

    if (decision_policy.compare("lcd")==0){
		decisor = new LCD();
    } else if (decision_policy.find("fix")==0){
		target_acceptance_ratio_string = decision_policy.substr(3);
		target_acceptance_ratio = atof( target_acceptance_ratio_string.c_str() );
		decisor = new Fix(target_acceptance_ratio);
    }
	//<aa>
	else if (decision_policy.find("ideal_blind")==0){
		decisor = new Ideal_blind(this);
    } else if (decision_policy.find("ideal_costaware")==0)
	{
		target_acceptance_ratio = 0; // I don't need this parameter
		decisor = new Ideal_costaware(target_acceptance_ratio, this );
	} else if (decision_policy.find("costaware")==0)
	{
		target_acceptance_ratio_string = decision_policy.substr( strlen("costaware") );
		target_acceptance_ratio = atof(target_acceptance_ratio_string.c_str());
		decisor = new Costaware(target_acceptance_ratio);
		check that only lru cache is acceptable
	}
	else if (decision_policy.compare("two_lru")==0)			// 2-LRU
	{
		name_cache_size = par("NC");
		decisor = new Two_Lru(name_cache_size);
	}
	//</aa>
	else if (decision_policy.find("btw")==0)				// Betweenness centrality
	{
		double db = getAncestorPar("betweenness");
		if (fabs(db - 1)<=0.001)
			error ("Node %i betwenness not defined.",getIndex());
		decisor = new Betweenness(db);
    }else if (decision_policy.find("prob_cache")==0)
	{
		decisor = new prob_cache(cache_slots);
    } else if (decision_policy.find("never")==0)
	{
		decisor = new Never();
	}
	//<aa>
    else if (decision_policy.compare("lce")==0 )
	{
		decisor = new Always();
	}

	if (decisor==NULL){
        std::stringstream ermsg; 
		ermsg<<"Decision policy \""<<decision_policy<<"\" incorrect";
	    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
    //} INITIALIZE DECISION POLICY

	// CHECKS{
		if ( decision_policy.find("fix")==0 || 
			( decision_policy.find("costaware")==0 && !decision_policy.find("ideal_costaware")== 0 )
		){	
			if ( strlen( target_acceptance_ratio_string.c_str() ) == 0 ){
				std::stringstream ermsg; 
				ermsg<<"You forgot to insert a valid value of acceptance rate when "<<
					"specifying the decision policy. Right examples are fix0.01, costaware0.1";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}

			if (target_acceptance_ratio <0){
				std::stringstream ermsg; 
				ermsg<<"target_acceptance_ratio "<<target_acceptance_ratio<<" is not valid. "<< 				
						"target_acceptance_ratio_string="<<target_acceptance_ratio_string<<
						"; decision_policy="<<decision_policy;
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}
		}
	// }CHECKS
	//</aa>




    //Cache statistics
    //--Average
    miss = 0;
    hit = 0;

	//<aa>
	decision_yes = decision_no = 0;
	//</aa>

    //--Per file
    cache_stats = new cache_stat_entry[__file_bulk + 1];

	//<aa>
	#ifdef SEVERE_DEBUG
	initialized = true;
	#endif
	//</aa>

}

void base_cache::initialize_cache_slots()
{
	if ( content_distribution::get_number_of_representations() > 1 )
	{
		std::stringstream ermsg; 
		ermsg<<"A generic cache cannot handle more than one representation"<< 
			"per object. Use some specific subclass in this case";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	cache_slots = par("C");
}


//<aa>
void base_cache::insert_into_cache(chunk_t chunk_id, 
			cache_item_descriptor* descr, unsigned storage_space)
{
	// All chunks must be indexed only based on object_id, chunk_number
	__srepresentation_mask(chunk_id, 0x0000);

	#ifdef SEVERE_DEBUG
		unordered_map<chunk_t,cache_item_descriptor *>::iterator it =
				find_in_cache(chunk_id);
		if (it != end_of_cache() &&
			__representation_mask(it->second->k) >= __representation_mask(descr->k)
		){
			std::stringstream ermsg; 
			ermsg<<"Representation "<< (__representation_mask(it->second->k) )<< "was already present, and you are "<<
				"trying to insert a lower representation "<<(__representation_mask(descr->k) )<<". This is forbidden"<<endl;
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		
		}

		ccn_data::check_representation_mask(descr->k);
	#endif

	update_occupied_slots(storage_space);
    cache[chunk_id] = descr;
	cout << "ciao: chunk "<< __id(cache[chunk_id]->k ) <<":"<<__chunk(cache[chunk_id]->k ) <<
		":"<<__representation_mask(cache[chunk_id]->k ) << " inserted."<<endl;
}

void base_cache::remove_from_cache(chunk_t chunk_id, unsigned storage_space)
{
	// All chunks must be indexed only based on object_id, chunk_number
	__srepresentation_mask(chunk_id, 0x0000);

 	cache.erase(chunk_id);
	update_occupied_slots(-1*storage_space);
}



unordered_map<chunk_t,cache_item_descriptor *>::iterator base_cache::find_in_cache(
			chunk_t chunk_id_without_representation_mask)
{
	#ifdef SEVERE_DEBUG
		if (__representation_mask(chunk_id_without_representation_mask) != 0x0000 )
		{
			std::stringstream ermsg; 
			ermsg<<"The identifier of the object you are searching for must be representation-agnostic, "<<
				"i.e. representation_mask should be zero";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	#endif

	unordered_map<chunk_t,cache_item_descriptor *>::iterator return_value = 
			cache.find(chunk_id_without_representation_mask);
	cout<<"ciao: base_cache::find_in_cache: looking for "<<chunk_id_without_representation_mask<<" found? "<<
		(return_value == end_of_cache() ? "no":"yes") << endl;
	return return_value;
}

unordered_map<chunk_t,cache_item_descriptor *>::iterator base_cache::end_of_cache()
{
	return cache.end();
}

unordered_map<chunk_t,cache_item_descriptor *>::iterator base_cache::beginning_of_cache()
{
	return cache.begin();
}


bool base_cache::full()
{
	return (get_occupied_slots() == get_slots());
}

void base_cache::update_occupied_slots(int increment)
{
	occupied_slots += increment;
}

unsigned base_cache::get_occupied_slots()
{
	return occupied_slots;
}
//</aa>

void base_cache::finish(){
    char name [30];
    sprintf ( name, "p_hit[%d]", getIndex());
    //Average hit rate
    recordScalar (name, hit * 1./(hit+miss));


    sprintf ( name, "hits[%d]", getIndex());
    recordScalar (name, hit );


    sprintf ( name, "misses[%d]", getIndex());
    recordScalar (name, miss);

	//<aa>
    sprintf ( name, "decision_yes[%d]", getIndex());
    recordScalar (name, decision_yes);

    sprintf ( name, "decision_no[%d]", getIndex());
    recordScalar (name, decision_no);

    sprintf ( name, "decision_ratio[%d]", getIndex());
	double decision_ratio = (decision_yes + decision_no == 0 ) ?
			0 : (double)decision_yes / (decision_yes + decision_no) ; 
    recordScalar (name, decision_ratio);

	if (statistics::record_cache_value)
	{
		sprintf ( name, "cache_value[%d]", getIndex());
		double cache_value = get_cache_value();
		recordScalar (name, cache_value);

		sprintf ( name, "average_price_of_cache[%d]", getIndex());
		double average_price = get_average_price();
		recordScalar (name, average_price);
	}

	decisor->finish(getIndex(), this);
	//</aa>

    //Per file hit rate
    sprintf ( name, "hit_node[%d]", getIndex());
    cOutVector hit_vector(name);
    for (uint32_t f = 1; f <= __file_bulk; f++)
        hit_vector.recordWithTimestamp(f, cache_stats[f].rate() );


}



// Returns true if the data has been stored
bool base_cache::data_store(ccn_data* data_msg)
{
	bool should_i_cache;
    if (cache_slots ==0){
		//<aa>
		after_discarding_data();
		//</aa>
		should_i_cache = false;
	}

    if (decisor->data_to_cache(data_msg ) )
	{
		//<aa>
		decision_yes++;
		//</aa>
		should_i_cache = true; 	// data_ store is an interface funtion:
							// each caching node should reimplement
							// that function

	}
	else
		decision_no++;
}


/*
 *    Storage handling of the received content ID inside the name cache (only with 2-LRU meta-caching).
 *
 *    Parameters:
 *    	 - elem: content ID to be stored.
 */
void base_cache::store_name(chunk_t elem)
{
    if (cache_slots ==0)
    {
		std::stringstream ermsg;
		ermsg<<" ALLERT! The size of the name cache is set to 0! Please check.";
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}

   data_store(elem);  // Store the content ID inside the Name Cache.
}

/*
 * 		Lookup function. The ID of the received Interest is looked up inside the local cache.
 * 		Hit/Miss statistics are gathered.
 *
 * 		Parameters:
 * 			- chunk: content ID of the received Interest.
 */
bool base_cache::lookup(chunk_t chunk )
{
    bool found = false;
    name_t name = __id(chunk);

    if (data_lookup(chunk))		// The requested content is cached locally.
   {
	//Average cache statistics(hit)
    	hit++;
    	found = true;

	//Per file cache statistics(hit)
	if (name <= __file_bulk)
	    cache_stats[name].hit++;

    }
   else		// The local cache does not contain the requested content.
   {
	       found = false;
		//Average cache statistics(miss)
		miss++;

		//Per file cache statistics(miss)
		if ( name <= __file_bulk )
			cache_stats[name].miss++;

   }
    return found;
}

/*
 * 	Lookup function without hit/miss statistics (used only with 2-LRU meta-caching to lookup the content ID inside the name cache).
 *
 * 	Parameters:
 * 		- chunk: content ID to be looked up.
 */
bool base_cache::lookup_name(chunk_t chunk )
{
    bool found = false;

    if (data_lookup(chunk))			// The content ID is present inside the name cache.
    	found = true;
    else
    	found = false;

    return found;
}

bool base_cache::fake_lookup(chunk_t chunk){
    return data_lookup(chunk);
}



//Clear all the statistics
void base_cache::clear_stat()
{
    hit = miss = 0; //local statistics

	//<aa>
	decision_yes = decision_no = 0;
	//</aa>
    delete cache_stats;
    cache_stats = new cache_stat_entry[__file_bulk+1];
}

/*
 *	Set the size of the local cache in terms of number of objects.
 *
 *	Parameters:
 *		- cSize: size of the cache.
 */
void base_cache::set_size(uint32_t cSize)
{
    std::stringstream ermsg; 
	ermsg<<"In this version of ccnsim, set_size(..) has been replaced by set_slots(..)";
	severe_error(__FILE__,__LINE__,ermsg.str().c_str() );

}

void base_cache::set_slots(unsigned slots_)
{
	cache_slots = slots_;
}


//<aa>
cache_item_descriptor* base_cache::data_lookup(chunk_t chunk)
{
	cache_item_descriptor* descr;
	representation_mask_t requested_mask = __representation_mask(chunk);	

	// All chunks must be indexed only based on object_id, chunk_number
	__srepresentation_mask(chunk, 0x0000);



	unordered_map<chunk_t,cache_item_descriptor *>::iterator it = find_in_cache(chunk);
    if ( it != end_of_cache() )
		descr = it->second;
	else 
		descr = NULL;

	cout<<"ciao: base cache looks for chunk "<<__id(chunk) <<":"<< __chunk(chunk) <<
		" at representation "<< __representation_mask(chunk) <<". Old representation found "<<
		(descr !=NULL ? __representation_mask(descr->k) :0 ) << endl;

	//{ REMOVE IT
	if (requested_mask>1 && descr ==NULL)
	{
		cout<<"cache content: ";
		unordered_map<chunk_t,cache_item_descriptor *>::iterator it_;
		for ( it_ = beginning_of_cache(); it_ != end_of_cache(); ++it_ )
		{
			chunk_t chunk_id = it_->second->k;
			cout<< __id(chunk_id) << ":" <<__chunk(chunk_id)<<":"<<__representation_mask(chunk_id)<<";    ";
		}
		cout<<endl;
		throw std::invalid_argument("TEMPORARY EXCEPTION. REMOVE IT. base_cache::data_lookup: You cannot receive repr 2 and not find a lower repr");
	}
	//} REMOVE IT

	return descr;
}

uint32_t base_cache::get_decision_yes()
{
	return decision_yes;	
}


uint32_t base_cache::get_decision_no(){
	return decision_no;
}
void base_cache::set_decision_yes(uint32_t n){
	decision_yes = n;
}
void base_cache::set_decision_no(uint32_t n){
	decision_no = n;
}

const DecisionPolicy* base_cache::get_decisor(){
	return decisor;
}

#ifdef SEVERE_DEBUG
bool base_cache::is_initialized()
{
	return initialized;
}
#endif
//</aa>
