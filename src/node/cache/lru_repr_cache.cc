//<aa>
#include "lru_repr_cache.h"
Register_Class(lru_repr_cache);

void lru_repr_cache::initialize()
{
	base_cache::initialize();

	// Retrieve the proactive component
	proactive_component = (client*) getParentModule()->getSubmodule("proactive_component");
}

void lru_repr_cache::if_chunk_is_present(chunk_t new_chunk_id, cache_item_descriptor* old)
{
	chunk_t old_chunk_id = old->k;
	old->k = new_chunk_id;
	unsigned old_storage = content_distribution::get_storage_space_of_chunk(old_chunk_id);
	unsigned new_storage = content_distribution::get_storage_space_of_chunk(new_chunk_id);
	update_occupied_slots(new_storage - old_storage);
}

cache_item_descriptor* lru_repr_cache::data_lookup(chunk_t chunk_id)
{
	cache_item_descriptor* stored = lru_cache::data_lookup(chunk_id);
	if (stored != NULL)
	{
		representation_mask_t mask = __representation_mask(stored->k);
		representation_mask_t intersection = mask & __representation_mask(chunk_id);
		if ( intersection == 0 )
			// The stored representation does not match with the requested ones
			return NULL;
		else{
			// A good chunk has been found
			unsigned short representation_found = content_distribution::get_representation_number(stored->k);
			
			if (representation_found < content_distribution::get_number_of_representations() )
			{
				// Try to retrieve a better representation of this chunk
				representation_mask_t improving_mask = 
					content_distribution::set_bit_to_zero(mask, representation_found);
				name_t object_id = __id(chunk_id);
				cnumber_t chunk_num = __chunk(chunk_id);
				proactive_component->request_specific_chunk(object_id, chunk_num, improving_mask);
			}
		}
	}
	return stored;
}
//</aa>
