//<aa>
#include "lru_repr_cache.h"
Register_Class(lru_repr_cache);

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
	cache_item_descriptor* old = lru_cache::data_lookup(chunk_t chunk_id);
	if (old != NULL)
	{
		representation_mask_t mask = __representation_mask(old->k);
		representation_mask_t intersection = mask & __representation_mask(chunk_id);
		if ( intersection == 0 )
			// The stored representation does not match with the requested ones
			return NULL;
		else{
			// A good chunk has been found
			unsigned short representation_found = content_distribution::get_representation_number(chunk_id);
			content_distribution::set_bit_to_zero();
		}
	}
	return old;
}
//</aa>
