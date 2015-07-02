//<aa>
#include "partitioned_cache.h"
Register_Class(partitioned_cache);

void partitioned_cache::initialize()
{
	base_cache::initialize();
	num_of_partitions = 1;
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
    unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
	return subcaches[repr-1]->data_lookup_receiving_data(chunk_id);
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
