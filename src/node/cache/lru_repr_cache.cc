//<aa>
Register_Class(lru_repr_cache);
#include "lru_repr_cache.h"

void lru_repr_cache::if_chunk_is_present(chunk_t new_chunk_id, cache_item_descriptor* old)
{
	chunk_t old_chunk_id = old->k;
	old->k = new_chunk_id;
	unsigned old_storage = content_distribution::get_storage_space_of_chunk(old_chunk_id);
	unsigned new_storage = content_distribution::get_storage_space_of_chunk(new_chunk_id);
	update_occupied_slots(new_storage - old_storage);
}
//</aa>
