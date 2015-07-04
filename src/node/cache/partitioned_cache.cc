//<aa>
#include "partitioned_cache.h"
#include "ccnsim.h"
Register_Class(partitioned_cache);

void partitioned_cache::initialize()
{
	base_cache::initialize();
	num_of_partitions = content_distribution::get_repr_h()->get_num_of_representations();
	subcaches = (lru_cache**) malloc(num_of_partitions * sizeof(lru_cache*) );

	//{ PARTITION SET UP
	// cache_slots is the number of chunk that we can store at high quality.
	unsigned possible_lowest_repr_chunks = cache_slots*
		content_distribution::get_repr_h()->get_storage_space_of_representation(num_of_partitions);

	unsigned subcache_size[num_of_partitions];
	for (unsigned short i=0; i<num_of_partitions; i++)
	{
		subcache_size[i] = (possible_lowest_repr_chunks / num_of_partitions)/
			content_distribution::get_repr_h()->get_storage_space_of_representation(i+1);
	}

	for (unsigned short i=0; i<num_of_partitions; i++)
	{
	    subcaches[i] = new lru_cache();
	    subcaches[i]->initialize_(std::string("lce"), subcache_size[i] );
	}
	//} PARTITION SET UP

	if (cache_slots > 0)
		// Retrieve the proactive component
		proactive_component = (client*) getParentModule()->getSubmodule("proactive_component");
	else
		proactive_component = NULL;
}

bool partitioned_cache::handle_data(ccn_data* data_msg, chunk_t& evicted)
{
	bool accept_new_chunk = base_cache::handle_data(data_msg, evicted);
	chunk_t chunk_id = data_msg->get_chunk_id();
	#ifdef SEVERE_DEBUG
		check_if_correct();
		content_distribution::get_repr_h()->check_representation_mask(chunk_id, CCN_D);
		if (evicted != 0)
			severe_error(__FILE__,__LINE__,"Evicted should be 0 now");
	#endif

	if (accept_new_chunk)
	{
		cache_item_descriptor* old = data_lookup_receiving_data(chunk_id);
		if (old == NULL)
		{
			unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
			lru_cache* subcache = subcaches[repr-1];

			#ifdef SEVERE_DEBUG
				unsigned occupied_before = subcache->get_occupied_slots();
			#endif

		    subcaches[repr-1]->handle_data(data_msg, evicted);

			#ifdef SEVERE_DEBUG
				if (occupied_before == 0 && evicted!=0)
					severe_error(__FILE__,__LINE__,"You evicted a ghost chunk");
			#endif


		    if (evicted != 0)
		    {
		    	chunk_t evicted_with_no_repr = evicted;
		    	__srepresentation_mask(evicted_with_no_repr, 0x0000);
		    	quality_map.erase(evicted_with_no_repr);
		    }
		    unsigned short incoming_representation =
		                    content_distribution::get_repr_h()->get_representation_number(chunk_id);
		    chunk_t chunk_id_without_repr = chunk_id;
		    __srepresentation_mask(chunk_id_without_repr, 0x0000);
		    quality_map[chunk_id_without_repr ] =incoming_representation;
		} else
		{
			//There is already a chunk that can replace the incoming one
			accept_new_chunk = false;
		}
	}
	#ifdef SEVERE_DEBUG
		check_if_correct();
	#endif

	return accept_new_chunk;
}

cache_item_descriptor* partitioned_cache::data_lookup_receiving_interest(chunk_t requested_chunk_id)
{
	- find the stored representation through quality map
	- See if it satisfies the interest. If yes, stored = that repr
	- Moreover, if yes, trigger the proactive component

    unsigned short repr = quality_map[....]
    cache_item_descriptor* stored = subcaches[repr-1]->data_lookup_receiving_interest(chunk_id);
    if (stored != NULL)
    {
    	// There is a chunk with the same [object_id, chunk_num] stored in the cache.
    	// I check if the stored representation can satisfy the interest
    	representation_mask_t request_mask = __representation_mask(requested_chunk_id);
    	representation_mask_t stored_mask = __representation_mask(stored->k);
    	representation_mask_t intersection = stored_mask & request_mask;
    	if ( intersection == 0 )
    		// The stored representation does not match with the requested ones
    		return NULL;
    	else
    		// A good chunk has been found
    		proactive_component->try_to_improve(stored->k, requested_chunk_id);
    }
    return stored;

}

cache_item_descriptor* partitioned_cache::data_lookup_receiving_data(chunk_t chunk_id)
{
	#ifdef SEVERE_ERROR
		check_if_correct();
	#endif

    cache_item_descriptor* good_already_stored_chunk;

    // Retrieve the current stored version of the chunk
    chunk_t chunk_id_without_repr = chunk_id;
    __srepresentation_mask(chunk_id_without_repr, 0x0000);
    unordered_map<chunk_t, unsigned short>::iterator it = quality_map.find(chunk_id_without_repr);

    if (it == quality_map.end() )
        // No chunk like the incoming one has been stored
        good_already_stored_chunk =  NULL;
    else{
        unsigned short old_representation = it->second;
        unsigned short incoming_representation =
                content_distribution::get_repr_h()->get_representation_number(chunk_id);
        if (incoming_representation > old_representation)
        {
            good_already_stored_chunk = NULL; // To signal that the new chunk must be stored

            // I remove the old representation
            remove_from_cache(new cache_item_descriptor(chunk_id) );
        }else{
        	// I do not have to store the incoming chunk, since I already have a better
        	// representation in cache
            good_already_stored_chunk =
                    subcaches[old_representation-1]->data_lookup_receiving_data(chunk_id);
            #ifdef SEVERE_DEBUG
                if (good_already_stored_chunk ==NULL)
                {
                    std::stringstream ermsg;
                    ermsg<<"quality_map said that a good chunk was stored in the cache partition of level "
                            <<old_representation<<" but looking into it I get a NULL pointer. This is an "
                            <<"error";
                    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
                }
            #endif
        }
    }
    return good_already_stored_chunk;
}

void partitioned_cache::remove_from_cache(cache_item_descriptor* descr)
{
    chunk_t chunk_id = descr->k;
    unsigned short representation=content_distribution::get_repr_h()->get_representation_number(chunk_id);

	// All chunks must be indexed only based on object_id, chunk_number
	__srepresentation_mask(chunk_id, 0x0000);

 	subcaches[representation-1]->remove_from_cache(descr);

 	quality_map.erase(chunk_id);
}


// Return true only if all the subcaches are full
bool partitioned_cache::full()
{
    for (unsigned short i=0; i<num_of_partitions; i++)
        if (!subcaches[i]->full() )
            return false;
    return true;
}

void partitioned_cache::finish()
{
	base_cache::finish();

	unsigned short num_of_repr = content_distribution::get_repr_h()->get_num_of_representations();

	std::stringstream breakdown_str;
	for (unsigned i = 0; i < num_of_repr; i++)
		breakdown_str << subcaches[i]->get_occupied_slots()<<":";
    char name [60];
    sprintf ( name, "representation_breakdown[%d] %s", getIndex(), breakdown_str.str().c_str());
    recordScalar (name, 0);
}


#ifdef SEVERE_DEBUG
void partitioned_cache::check_if_correct()
{
	base_cache::check_if_correct();
	unsigned stored_chunks = 0;

	for (unsigned short i=0; i<num_of_partitions; i++)
	{
	    subcaches[i]->check_if_correct();
	    stored_chunks += subcaches[i]->get_occupied_slots();
	}

	//{ CHECK quality_map CONSISTENCY
	if (stored_chunks != quality_map.size() )
	{
		std::stringstream ermsg;
		ermsg<<"stored_chunks="<<stored_chunks<<" while quality_map.size()="<<quality_map.size();
		severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}

	for(unordered_map<chunk_t, unsigned short>::iterator it = quality_map.begin(); it != quality_map.end(); it++)
	{
		chunk_t chunk_id_with_no_repr = it->first;
		if (__representation_mask(chunk_id_with_no_repr ) != 0x0000 )
			severe_error(__FILE__,__LINE__,"All objects must be indexed in a representation unaware fashon in the quality map");

		unsigned short representation = it->second;

		if ( !subcaches[representation-1]->fake_lookup(chunk_id_with_no_repr) )
			severe_error(__FILE__,__LINE__,"Inconsistency of quality_map");
	}
	//} CHECK quality_map CONSISTENCY
}

void partitioned_cache::dump()
{
	cout<<"caches: ";
	for (unsigned short i=0; i<num_of_partitions; i++)
		cout<<subcaches[i]->get_cache_content()<<" ; ";
	cout<<endl;

	cout<<"quality map: ";
	for (unordered_map<chunk_t, unsigned short>::iterator it = quality_map.begin(); it != quality_map.end(); it++ )
		cout<<__id(it->first)<<":"<<__chunk(it->first)<<":"<<__representation_mask(it->first)<<" ;";
	cout<<endl;
}

void partitioned_cache::check_representation_compatibility(){}
#endif

//</aa
