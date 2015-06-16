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
#include "lru_cache.h"

//<aa>
#include "error_handling.h"
#include "costaware_ancestor_policy.h"
#include "statistics.h"
//</aa>

Register_Class(lru_cache);

//<aa>
bool lru_cache::is_it_empty() const
{
	//{ CHECK
		#ifdef SEVERE_DEBUG
		if ( (lru_==NULL && mru_!=NULL ) || (lru_!=NULL && mru_==NULL ) )
		{
			std::stringstream ermsg; 
			ermsg<<"Error in pointer updarte";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif
	// }CHECK

	if (lru_==NULL)
		return true;
	else return false;
}
//</aa>

void lru_cache::data_store(chunk_t chunk_id)
{
	unsigned storage_space = content_distribution::get_storage_space_of_chunk(chunk_id);

	// All chunks must be indexed only based on object_id, chunk_number
	chunk_t chunk_id_without_representation_mask = chunk_id;
	__srepresentation_mask(chunk_id_without_representation_mask, 0x0000);

	
    //When the element is already stored within the cache, simply update the 
    //position of the element within the list and exit
    if (data_lookup(chunk_id_without_representation_mask) )
		return;


	cache_item_descriptor *p = new cache_item_descriptor();//position for the new element
							//<aa> i.e. datastructure for the new element </aa>
    p->k = chunk_id;// <aa> We store the complete chunk_id because we need to check the 
					// 		representation_mask later, when a request arrives
					// </aa>

    p->hit_time = simTime();
    p->newer = 0;
    p->older = 0;

    // The cache is empty. Add just one element if it fits into the cache space. 
	// The mru and lru element are the same
    if ( is_it_empty() && ( actual_size + storage_space) <= (unsigned)get_slots() )
	{
        actual_size += storage_space;
        lru_ = p;
		mru_ = p;
        cache[chunk_id_without_representation_mask] = p;
        return;
    } 

    //The cache is not empty. The new element is the newest. Add in the front
    //of the list
    p->older = mru_; // mru swaps in second position (in terms of utilization rank)
    mru_->newer = p; // update the newer element for the secon newest element
    mru_ = p; //update the mru (which becomes that just inserted)

	actual_size += storage_space;
	//<aa> I transformed an if in a loop </aa>
    while (actual_size  > get_slots() )
	{
        //if the cache is full, delete the last element
        //
        chunk_t evicted_chunk_id = lru_->k;

		//<aa>
		// All chunks must be indexed only based on object_id, chunk_number
		chunk_t evicted_chunk_id_without_representation_mask = evicted_chunk_id;
		__srepresentation_mask(evicted_chunk_id_without_representation_mask, 0x0000);
		//</aa>

        cache_item_descriptor *tmp = lru_;
        lru_ = tmp->newer;//the new lru is the element before the least recently used

        lru_->older = 0; //as it is still in memory for a while set the actual lru point to null (CHECK this)
        tmp->older = 0;
        tmp->newer = 0;

        free(tmp);
        cache.erase(evicted_chunk_id_without_representation_mask); //erase from the cache the most unused element
		actual_size = actual_size - content_distribution::get_storage_space_of_chunk(evicted_chunk_id);
    }

    cache[chunk_id_without_representation_mask] = p; //store the new element together with its position
}

void lru_cache::set_price_to_last_inserted_element(double price)
{
	#ifdef SEVERE_DEBUG	
	if ( !statistics::record_cache_value )
	{
			std::stringstream ermsg; 
			ermsg<<"statistics::record_cache_value="<<statistics::record_cache_value<<": ";
			ermsg<<"set_price_to_last_inserted_element(..) is useful when you want to record "<<
				" the cache_value; when statistics::record_cache_value is disabled this method "<<
				" may be useless. Make sure you really need this method. If yes, disable this"<<
				" error.If not, try to rethink your code";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	#endif
	mru_->set_price(price);
}

//<aa>
cache_item_descriptor* lru_cache::get_mru(){
	#ifdef SEVERE_DEBUG
	if (statistics::record_cache_value && actual_size > 0){		
		mru_->get_price(); // to verify whether the price is correctly set up
	}
	#endif	

	return mru_;
}

cache_item_descriptor* lru_cache::get_lru(){
	#ifdef SEVERE_DEBUG
	if (statistics::record_cache_value ){
		lru_->get_price(); // to verify whether the price is correctly set up
	}
	#endif

	return lru_;
}

void lru_cache::set_lru(cache_item_descriptor* new_lru)
{
	lru_ = new_lru;
}

void lru_cache::set_mru(cache_item_descriptor* new_mru)
{
	mru_ = new_mru;
}


const cache_item_descriptor* lru_cache::get_eviction_candidate(){
	if ( full() ) 
		return get_lru();
	else return NULL;
}

//</aa>

bool lru_cache::fake_lookup(chunk_t chunk_id)
{
	// Each chunk is indexed only based on its object_id and chunk_number.
	chunk_t chunk_id_without_representation_mask = chunk_id;
	__srepresentation_mask(chunk_id_without_representation_mask, 0x0000);

    unordered_map<chunk_t,cache_item_descriptor *>::iterator it = cache.find(chunk_id_without_representation_mask);
    //look for the elements
    if (it==cache.end()){
		//if not found return false and do nothing
		return false;
    }else 
		return true;
}

bool lru_cache::data_lookup(chunk_t chunk_id)
{
	// Each chunk must be indexed only based on its object_id and chunk_number.
	chunk_t chunk_id_without_representation_mask = chunk_id;
	__srepresentation_mask(chunk_id_without_representation_mask, 0x0000);

    //updating an element is just a matter of manipulating the list
    unordered_map<chunk_t,cache_item_descriptor *>::iterator it = cache.find(chunk_id_without_representation_mask);

    //
    //look for the elements
    if (it==cache.end())
	{
		//if not found return false and do nothing
		return false;
    }

    cache_item_descriptor* pos_elem = it->second;
	if ( (__representation_mask(pos_elem->k) & __representation_mask(chunk_id) ) == 0 )
		// The stored representation does not match with the requested ones
		return false;

    // If content matched, update the position
    if (pos_elem->older && pos_elem->newer){
        //if the element is in the middle remove the element from the list
        pos_elem->newer->older = pos_elem->older;
        pos_elem->older->newer = pos_elem->newer;
    }else if (!pos_elem->newer){
        //if the element is the mru
        return true; //do nothing, return true
    } else{
        //if the element is the lru, remove the element from the bottom of the list
        set_lru(pos_elem->newer);
        get_lru()->older = 0;
    }


    //Place the elements as in front of the position list (it's the newest one)
    pos_elem->older = get_mru();
    pos_elem->newer = 0;
    get_mru()->newer = pos_elem;

    //update the mru
    set_mru(pos_elem);
    get_mru()->hit_time = simTime();
    return true;
}


void lru_cache::dump(){
    cache_item_descriptor *it = get_mru();
    int p = 1;
    while (it){
	cout<<p++<<" ]"<< __id(it->k)<<"/"<<__chunk(it->k)<<endl;
	it = it->older;
    }
}

//<aa>
// {STATISTICS
	double lru_cache::get_cache_value()
	{
		#ifdef SEVERE_DEBUG	
		if ( !statistics::record_cache_value )
		{
				std::stringstream ermsg; 
				ermsg<<"get_cache_value(..) is useful when you want to record "<<
					" the cache_value; when statistics::record_cache_value is disabled this method "<<
					" may be useless. Make sure you really need this method. If yes, disable this"<<
					" error.If not, try to rethink your code";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif

		double value = 0;

		WeightedContentDistribution* content_distribution_module = 
			Costaware_ancestor::get_weighted_content_distribution_module();

		cache_item_descriptor *it = get_mru();
		int p = 1;
		while (it){
			chunk_t object_index = it->k;
			double alpha = content_distribution_module->get_alpha();
			double price = it->get_price();
			double weight = Costaware_ancestor::compute_content_weight(object_index,price,alpha);
			value += weight;
		
	//		cout<< p <<" ] "<< object_index <<" : "<< price << " : "<< weight << endl;
			p++;
			it = it->older;
		}

		return value;
	}

	double lru_cache::get_average_price()
	{
		#ifdef SEVERE_DEBUG	
		if ( !statistics::record_cache_value )
		{
				std::stringstream ermsg; 
				ermsg<<"get_average_price(..) is useful when you want to record "<<
					" the cache_value; when statistics::record_cache_value is disabled this method "<<
					" may be useless. Make sure you really need this method. If yes, disable this"<<
					" error.If not, try to rethink your code";
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
		#endif

		double sum_of_prices = 0;

		cache_item_descriptor *it = get_mru();
		int counter = 0;
		while (it){
			double price = it->get_price();
			sum_of_prices += price;
			it = it->older;
			counter++;
		}

		double average_price = sum_of_prices / counter;
		return average_price;
	}
	//</aa>


	bool lru_cache::full()
	{
		return ( actual_size == (unsigned) get_slots());
	}
// }STATISTICS
