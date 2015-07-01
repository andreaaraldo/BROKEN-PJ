//<aa>
#include "partitioned_cache.h"
Register_Class(partitioned_cache);

void partitioned_cache::initialize()
{
	base_cache::initialize();
	unsigned short num_of_partitions = 1;
	subcaches = (lru_cache**) malloc(num_of_partitions * sizeof(lru_cache*) );
	for (unsigned short i=0; i<num_of_partitions; i++)
	{
	    subcaches[i] = new lru_cache();
	    subcaches[i]->
	    subcaches[i]->initialize();
	}
}

bool partitioned_cache::data_store(ccn_data* data_msg)
{
    bool accept_new_chunk = base_cache::data_store(data_msg);
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
            // There is no chunk already stored that can replace the incoming one.
            // We need to store the incoming one.
            unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
            subcaches[repr-1]->data_store(data_msg);
        } else
        {
            //There is already a chunk that can replace the incoming one
            accept_new_chunk = false;
        }
    }
    return accept_new_chunk;
}


//</aa
