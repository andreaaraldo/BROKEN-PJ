//<aa>
#include "partitioned_cache.h"
#include "ccnsim.h"
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

bool partitioned_cache::handle_data(ccn_data* data_msg, chunk_t& evicted)
{
	bool accept_new_chunk = base_cache::handle_data(data_msg, evicted);
	chunk_t chunk_id = data_msg->get_chunk_id();
	#ifdef SEVERE_DEBUG
		check_if_correct();
		content_distribution::get_repr_h()->check_representation_mask(chunk_id, CCN_D);
	#endif
	cout<<"ciao: incoming object is "<<__id(chunk_id)<<":"<<__chunk(chunk_id)<<":"<<__representation_mask(chunk_id)<<endl;

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

		    cout<<"ciao: subcache was "<<occupied_before<<" before. ";
		    subcaches[repr-1]->handle_data(data_msg, evicted);

			#ifdef SEVERE_DEBUG
				if (occupied_before == 0 && evicted!=0)
					severe_error(__FILE__,__LINE__,"You evicted a ghost chunk");
			#endif


		    cout<<"evicted is "<<__id(evicted)<<":"<<__chunk(evicted)<<":"<<__representation_mask(evicted)<<" and after eviction subcache is "<<subcaches[repr-1]->get_occupied_slots()<<endl;
		    if (evicted != 0)
		    {
		    	chunk_t evicted_with_no_repr = evicted;
		    	__srepresentation_mask(evicted_with_no_repr, 0x0000);
		    	cout<<"ciao quality map was "<<quality_map.size()<<" before and after erasing is ";
		    	quality_map.erase(evicted_with_no_repr);
		    	cout<<quality_map.size()<<endl;
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

cache_item_descriptor* partitioned_cache::data_lookup_receiving_interest(chunk_t chunk_id)
{
    unsigned short repr = content_distribution::get_repr_h()->get_representation_number(chunk_id);
	return subcaches[repr-1]->data_lookup_receiving_interest(chunk_id);
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
	cout<<"ciao: caches: ";
	for (unsigned short i=0; i<num_of_partitions; i++)
		cout<<subcaches[i]->get_cache_content()<<" ; ";
	cout<<endl;

	cout<<"ciao: quality map: ";
	for (unordered_map<chunk_t, unsigned short>::iterator it = quality_map.begin(); it != quality_map.end(); it++ )
		cout<<__id(it->first)<<":"<<__chunk(it->first)<<":"<<__representation_mask(it->first)<<" ;";
	cout<<endl;
}
#endif

//</aa
