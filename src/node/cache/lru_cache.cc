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


void lru_cache::data_store(chunk_t elem){
    //When the element is already stored within the cache, simply update the 
    //position of the element within the list and exit
    if (data_lookup(elem))
	return;

    lru_pos *p = (lru_pos *)malloc (sizeof(lru_pos)); //position for the new element
										//<aa> i.e. datastructure for the new element </aa>
    //lru_pos *p = new lru_pos();
    p->k = elem;
    p->hit_time = simTime();
    p->newer = 0;
    p->older = 0;

    //The cache is empty. Add just one element. The mru and lru element are the
    //same
    if (actual_size == 0){
        actual_size++;
        set_lru(p);
		set_mru(p);
        cache[elem] = p;
        return;
    } 

    //The cache is not empty. The new element is the newest. Add in the front
    //of the list
    p->older = get_mru(); // mru swaps in second position (in terms of utilization rank)
    get_mru()->newer = p; // update the newer element for the secon newest element
    set_mru(p); //update the mru (which becomes that just inserted)

    if (actual_size==get_size()){
        //if the cache is full, delete the last element
        //
        chunk_t k = get_lru()->k;
        lru_pos *tmp = get_lru();
        set_lru(tmp->newer);//the new lru is the element before the least recently used

        get_lru()->older = 0; //as it is still in memory for a while set the actual lru point to null (CHECK this)
        tmp->older = 0;
        tmp->newer = 0;

        free(tmp);
        cache.erase(k); //erase from the cache the most unused element
    }else
        //otherwise do nothing, just update the actual_size of the cache
        actual_size++;

    cache[elem] = p; //store the new element together with its position
}

void lru_cache::set_price_to_last_inserted_element(double price)
{
	get_mru()->set_price(price);
}

//<aa>
lru_pos* lru_cache::get_mru(){
	#ifdef SEVERE_DEBUG
	if (statistics::record_cache_value && actual_size > 0){		
		mru_->get_price(); // to verify whether the price is correctly set up
	}
	#endif	

	return mru_;
}

lru_pos* lru_cache::get_lru(){
	#ifdef SEVERE_DEBUG
	if (lru_ != NULL){
		// To see if a seg fault arises due to the access to a forbidden area
		// To use with valgrind software
		chunk_t test = lru_->k;
	} //else the cache is empty

	if (statistics::record_cache_value ){
		lru_->get_price(); // to verify whether the price is correctly set up
	}
	#endif

	return lru_;
}

void lru_cache::set_lru(lru_pos* new_lru)
{
	lru_ = new_lru;
}

void lru_cache::set_mru(lru_pos* new_mru)
{
	mru_ = new_mru;
}


const lru_pos* lru_cache::get_eviction_candidate(){
	if ( full() ) 
		return get_lru();
	else return NULL;
}

//</aa>

bool lru_cache::fake_lookup(chunk_t elem){
//    if (getIndex()==12)
//	return true;
    unordered_map<chunk_t,lru_pos *>::iterator it = cache.find(elem);
    //look for the elements
    if (it==cache.end()){
	//if not found return false and do nothing
	return false;

    }else 
	return true;
}

bool lru_cache::data_lookup(chunk_t elem){
    //updating an element is just a matter of manipulating the list
    unordered_map<chunk_t,lru_pos *>::iterator it = cache.find(elem);

    //
    //look for the elements
    if (it==cache.end()){
	//if not found return false and do nothing
	return false;

    }

    lru_pos* pos_elem = it->second;
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
    lru_pos *it = get_mru();
    int p = 1;
    while (it){
	cout<<p++<<" ]"<< __id(it->k)<<"/"<<__chunk(it->k)<<endl;
	it = it->older;
    }
}

//<aa>
double lru_cache::get_cache_value()
{
	WeightedContentDistribution* content_distribution_module = 
		Costaware_ancestor::get_weighted_content_distribution_module();
	double value = 0;

    lru_pos *it = get_mru();
    int p = 1;
    while (it){
		chunk_t object_index = it->k;
		double alpha = content_distribution_module->get_alpha();
		double price = it->get_price();
		double weight = Costaware_ancestor::compute_content_weight(object_index,price,alpha);
		value += weight;
		
		cout<< p <<" ] "<< object_index <<" : "<< price << " : "<< weight << endl;
		p++;
		it = it->older;
    }

	return value;
}
//</aa>


bool lru_cache::full(){
    return (actual_size==get_size());
}
