//<aa>
#include "partitioned_cache.h"
Register_Class(partitioned_cache);

void partitioned_cache::initialize()
{
	base_cache::initialize();
	num_of_partitions = content_distribution::get_repr_h()->get_num_of_representations();
	subcaches = (lru_cache**) malloc(num_of_partitions * sizeof(lru_cache*) );
	for (unsigned short i=0; i<num_of_partitions; i++)
	{
	    subcaches[i] = new lru_cache();
	    subcaches[i]->initialize_(std::string("lce"), cache_slots/num_of_partitions);
	}
}

bool partitioned_cache::handle_data(ccn_data* data_msg)
{
	bool accept_new_chunk = base_cache::handle_data(data_msg);
	chunk_t chunk_id = data_msg->get_chunk_id();
	#ifdef SEVERE_DEBUG
		check_if_correct();
		content_distribution::get_repr_h()->check_representation_mask(chunk_id, CCN_D);
	#endif

	if (accept_new_chunk)
	{
		cache_item_descriptor* old = data_lookup_receiving_data(chunk_id);
		if (old == NULL)
		{
		    unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
		    subcaches[repr-1]->handle_data(data_msg);
		    unsigned short incoming_representation =
		                    content_distribution::get_repr_h()->get_representation_number(chunk_id);
		    chunk_t chunk_id_without_repr = chunk_id;
		    __srepesentation_mask(chunk_id_without_repr, 0x0000);
		    quality_map[chunk_id_without_repr ] =incoming_representation;
		} else
		{
			//There is already a chunk that can replace the incoming one
			accept_new_chunk = false;
		}
	}
	return accept_new_chunk;
}

cache_item_descriptor* partitioned_cache::data_lookup_receiving_interest(chunk_t chunk_id)
{
    unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
	return subcaches[repr-1]->data_lookup_receiving_interest(chunk_id);
}

cache_item_descriptor* partitioned_cache::data_lookup_receiving_data(chunk_t chunk_id)
{
    cache_item_descriptor* good_already_stored_chunk;

    // Retrieve the current stored version of the chunk
    chunk_t chunk_id_without_repr = chunk_id;
    __srepesentation_mask(chunk_id_without_repr, 0x0000);
    unordered_map<chunk_t, unsigned short>::iterator* it = quality_map::find(chunk_id_without_repr);

    if (it == quality_map::end() )
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
            remove_from_cache(chunk_id);
        }else{
            good_already_stored_chunk =
                    subcaches[old_representation-1]->data_lookup_receving_data(chunk_id);
            #ifdef SEVERE_DEBUG
                if (good_already_stored_chunk ==NULL)
                {
                    std:stringstream ermsg;
                    ermsg<<"quality_map said that a good chunk was stored in the cache partition of level "
                            <<old_representation<<" but looking into it I get a NULL pointer. This is an "
                            <<error;
                    severe_debug(__FILE__,__LINE__,srmsg.str().c_str() );
                }
            #endif
        }
    }
    return good_already_stored_chunk;
}

void partitioned_cache::remove_from_cache(chunk_t chunk_id, unsigned storage_space)
{
    unsigned short representation=content_distribution::get_repr_h()->get_representation_number(chunk_id);

	// All chunks must be indexed only based on object_id, chunk_number
	__srepresentation_mask(chunk_id, 0x0000);

 	subcaches[representation-1]->remove_from_cache(chunk_t chunk_id, unsigned storage_space);

 	quality_map.erase[chunk_id];
}


// Return true only if all the subcaches are full
bool partitioned_cache::full()
{
    for (unsigned short i=0; i<num_of_partitions; i++)
        if (!subcaches[i]->full() )
            return false;
    return true;
}
//</aa
