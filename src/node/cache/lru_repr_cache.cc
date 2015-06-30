//<aa>
#include "lru_repr_cache.h"
Register_Class(lru_repr_cache);

void lru_repr_cache::initialize()
{
	base_cache::initialize();

	if (get_slots() > 0)
		// Retrieve the proactive component
		proactive_component = (client*) getParentModule()->getSubmodule("proactive_component");
	else
		proactive_component = NULL;

}

void lru_repr_cache::initialize_cache_slots()
{
    int chunks_at_highest_representation = par("C");
	unsigned highest_representation_space = content_distribution::get_repr_h()->get_storage_space_of_representation(
		content_distribution::get_repr_h()->get_number_of_representations() );
	cache_slots = (unsigned) chunks_at_highest_representation * highest_representation_space;
}

cache_item_descriptor* lru_repr_cache::data_lookup_receiving_interest(chunk_t requested_chunk_id)
{
	cache_item_descriptor* stored = lru_cache::data_lookup_receiving_interest(requested_chunk_id);
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
		else{
			// A good chunk has been found
			unsigned short representation_found = content_distribution::get_repr_h()->get_representation_number(stored->k);
			
			// Try to retrieve a better representation of this chunk
			representation_mask_t improving_mask = (0xFFFF << representation_found );
			improving_mask = (improving_mask & request_mask);

			#ifdef SEVERE_DEBUG
				if( (improving_mask & 0x0001) != 0 )
				{
					std::stringstream ermsg; 
					ermsg<< "An improving mask cannot ask for repr 1. This would not be an improvement."<<
						"request_mask="<<request_mask<<"; stored_mask="<<stored_mask<<"; representation_found="<<
						representation_found;
					severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
				}
			#endif

			if (improving_mask != 0x0000)
			{	// There is a higher representation and I want to retrieve it
				name_t object_id = __id(requested_chunk_id);
				cnumber_t chunk_num = __chunk(requested_chunk_id);
				proactive_component->request_specific_chunk_from_another_class(
											object_id, chunk_num, improving_mask);
			}
		}
	}
	return stored;
}

cache_item_descriptor* lru_repr_cache::data_lookup_receiving_data(chunk_t incoming_chunk_id)
{
	cache_item_descriptor* stored = lru_cache::data_lookup_receiving_data(incoming_chunk_id);
	if (stored != NULL)
	{
		// There is a chunk with the same [object_id, chunk_num] stored in the cache.
		// It has already been put at the head of the position list
		chunk_t old_chunk_id = stored->k;
		representation_mask_t incoming_mask = __representation_mask(incoming_chunk_id);
		representation_mask_t stored_mask = __representation_mask(old_chunk_id);
		if (stored_mask < incoming_mask )
		{	// The incoming representation is better than the stored one.
			remove_from_cache(stored);
			stored = NULL; // To signal that the new chunk must be stored
		}
	}
	#ifdef SEVERE_DEBUG
	check_if_correct();
	#endif
	return stored;
}

void lru_repr_cache::finish()
{
	lru_cache::finish();

	//{ COMPUTE REPRESENTATION BREAKDOWN
	unsigned short num_of_repr = content_distribution::get_repr_h()->get_number_of_representations();
	unsigned* breakdown = (unsigned*)calloc(content_distribution::get_repr_h()->get_number_of_representations(), sizeof(unsigned) );
	unordered_map<chunk_t,cache_item_descriptor *>::iterator it;
	for ( it = beginning_of_cache(); it != end_of_cache(); ++it )
	{
		chunk_t chunk_id = it->second->k;
	    breakdown[content_distribution::get_repr_h()->get_representation_number(chunk_id)-1]++;
	}

	std::stringstream breakdown_str;
	for (unsigned i = 0; i < num_of_repr; i++)
		breakdown_str << breakdown[i]<<":";
    char name [60];
    sprintf ( name, "representation_breakdown[%d] %s", getIndex(), breakdown_str.str().c_str());
    recordScalar (name, 0);
	//} COMPUTE REPRESENTATION BREAKDOWN
}

//</aa
