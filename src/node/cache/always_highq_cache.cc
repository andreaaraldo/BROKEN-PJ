//<aa>
#include "always_highq_cache.h"
#include "ccnsim.h"

Register_Class(always_highq_cache);

void always_highq_cache::initialize()
{
	base_cache::initialize();
	num_of_partitions = content_distribution::get_repr_h()->get_num_of_representations();
	subcaches = (lru_cache**) malloc(num_of_partitions * sizeof(lru_cache*) );

	//{ PARTITION SET UP
	// The only non null subcache is the one for highest representation
	subcaches[num_of_partitions-1] = new lru_cache();
	subcaches[num_of_partitions-1]->initialize_(std::string("lce"), cache_slots );

	for (unsigned short i=0; i<num_of_partitions-1; i++)
	    subcaches[i] = NULL;
	//} PARTITION SET UP

	if (cache_slots > 0)
		// Retrieve the proactive component
		proactive_component = (ProactiveComponent*) getParentModule()->getSubmodule("proactive_component");
	else
		proactive_component = NULL;
}

bool always_highq_cache::handle_data(ccn_data* data_msg, chunk_t& evicted)
{
	bool accept_new_chunk = base_cache::handle_data(data_msg, evicted);
	chunk_t chunk_id = data_msg->get_chunk_id();
	unsigned short incoming_repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
	if (incoming_repr != num_of_partitions)
		// This is not a chunk at the highest representation, thus I cannot store it
		accept_new_chunk = false;

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
			// No chunk with the same [object_id, chunk_num] is in cache
			#ifdef SEVERE_DEBUG
				unsigned occupied_before =
					subcaches[incoming_repr-1]->get_occupied_slots();
			#endif

			accept_new_chunk = subcaches[incoming_repr-1]->handle_data(data_msg, evicted);

		    #ifdef SEVERE_DEBUG
		    	if (occupied_before == 0 && evicted!=0)
					severe_error(__FILE__,__LINE__,"You evicted a ghost chunk");
			#endif

		    if (evicted != 0)
		    {
				#ifdef SEVERE_DEBUG
		    		if (	content_distribution::get_repr_h()->get_representation_number(evicted) !=
		    				num_of_partitions)
		    			severe_error(__FILE__,__LINE__,"A chunk with representation different from the highest one has been found");
				#endif
		    	chunk_t evicted_with_no_repr = evicted;
		    	__srepresentation_mask(evicted_with_no_repr, 0x0000);
		    	quality_map.erase(evicted_with_no_repr);
		    }
		} else
			//There is already the chunk
			accept_new_chunk = false;

		if (accept_new_chunk)
		{
			#ifdef SEVERE_DEBUG
    			if (incoming_repr != num_of_partitions)
    			severe_error(__FILE__,__LINE__,"Trying to accept a representation different from the highest one");
			#endif
    		chunk_t chunk_id_without_repr = chunk_id;
		    __srepresentation_mask(chunk_id_without_repr, 0x0000);
		    quality_map[chunk_id_without_repr ] =incoming_repr;
		}
	}
	#ifdef SEVERE_DEBUG
		check_if_correct();
	#endif

	return accept_new_chunk;
}

cache_item_descriptor* always_highq_cache::data_lookup_receiving_interest(chunk_t requested_chunk_id)
{
	cache_item_descriptor* stored = NULL;
	chunk_t chunk_id_without_repr = requested_chunk_id;
	__srepresentation_mask(chunk_id_without_repr, 0x0000);
	unordered_map<chunk_t, unsigned short>::iterator it = quality_map.find(chunk_id_without_repr);

	if (it != quality_map.end() )
	{
		unsigned short stored_representation = it->second;
		stored = subcaches[stored_representation-1]->data_lookup_receiving_interest(requested_chunk_id);

		#ifdef SEVERE_DEBUG
			if ( !content_distribution::get_repr_h()->is_it_compatible(stored->k, requested_chunk_id) )
				severe_error(__FILE__,__LINE__,"Found a stored object that is not compatible.");
		#endif
	}else
	{
		// The requested chunks has not been found. I proactively request the highest representation
		proactive_component->request_specific_chunk_from_another_class(
							__id(requested_chunk_id), __chunk(requested_chunk_id),
							0x0001 << (num_of_partitions-1)
		);
	}
	return stored;
}

//</aa
