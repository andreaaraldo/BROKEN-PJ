/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <cmath>
#include "base_cache.h"
#include "core_layer.h"
#include "statistics.h"
#include "content_distribution.h"
#include "ccn_data_m.h"

#include "fix_policy.h"
//<aa>
#include "costprob_policy.h"
#include "error_handling.h"
//</aa>
#include "lcd_policy.h"
#include "never_policy.h"
#include "always_policy.h"
#include "decision_policy.h"
#include "betweenness_centrality.h"
#include "prob_cache.h"

#include "ccnsim.h"


//Initialization function
void base_cache::initialize(){

    nodes      = getAncestorPar("n");
    level = getAncestorPar("level");
    cache_size = par("C");  //cache size


    string decision_policy = par("DS");
    //Initialize the storage policy
    if (decision_policy.compare("lcd")==0){
		decisor = new LCD();
    } else if (decision_policy.find("fix")==0){
		string sp = decision_policy.substr(3);
		double dp = atof(sp.c_str());
		decisor = new Fix(dp);
    }
	//<aa>
	else if (decision_policy.find("costprob")==0){
		string sens_string = decision_policy.substr( strlen("costprob") );
		double sens = atof(sens_string.c_str());
		decisor = new Costprob(sens);

		std::stringstream ermsg; 
		//Check for consistency
		    if (sens>1 || sens <0){
				ermsg<<"sens "<<sens<<" is not valid. sens_string="<<sens_string<<
							"; decision_policy="<<decision_policy;
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}

		#ifdef SEVERE_DEBUG
		ermsg<<"costprob will be used with the sensitivity "<<sens;
	    debug_message(__FILE__,__LINE__,ermsg.str().c_str() );
		#endif
    }
	//</aa>
	 else if (decision_policy.find("btw")==0){
		double db = getAncestorPar("betweenness");
		if (fabs(db - 1)<=0.001)
			error ("Node %i betwenness not defined.",getIndex());
		decisor = new Betweenness(db);
    }else if (decision_policy.find("prob_cache")==0){
		decisor = new prob_cache(cache_size);
    } else if (decision_policy.find("never")==0)
		decisor = new Never();
	//<aa>
    else if (decision_policy.compare("lce")==0 )
		decisor = new Always();
	else{
        std::stringstream ermsg; 
		ermsg<<"Decision policy incorrect";
	    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
	//</aa>


    //Cache statistics
    //--Average
    miss = 0;
    hit = 0;
    //--Per file
    cache_stats = new cache_stat_entry[__file_bulk + 1];

}

void base_cache::finish(){
    char name [30];
    sprintf ( name, "p_hit[%d]", getIndex());
    //Average hit rate
    recordScalar (name, hit * 1./(hit+miss));


    sprintf ( name, "hits[%d]", getIndex());
    recordScalar (name, hit );


    sprintf ( name, "misses[%d]", getIndex());
    recordScalar (name, miss);

    //Per file hit rate
    sprintf ( name, "hit_node[%d]", getIndex());
    cOutVector hit_vector(name);
    for (uint32_t f = 1; f <= __file_bulk; f++)
        hit_vector.recordWithTimestamp(f, cache_stats[f].rate() );


}



//Base class function: a data has been received:
void base_cache::store(cMessage *in){
    if (cache_size ==0)
	return;

    if (decisor->data_to_cache((ccn_data*)in ) )
	data_store( ( (ccn_data* ) in )->getChunk() ); //store is an interface funtion: each caching node should reimplement that function

}



//Base class function: lookup for a given data
//it wraps statistics on misses and hits
bool base_cache::lookup(chunk_t chunk ){
    bool found = false;
    name_t name = __id(chunk);

    if (data_lookup(chunk)){
	//Average cache statistics(hit)
	hit++;
	found = true;

	//Per file cache statistics(hit)
	if (name <= __file_bulk)
	    cache_stats[name].hit++;

    }else{
        found = false;

		//Average cache statistics(miss)
		miss++;
		//Per file cache statistics(miss)
		if ( name <= __file_bulk )
			cache_stats[name].miss++;
    }

    return found;

}

bool base_cache::fake_lookup(chunk_t chunk){
    return data_lookup(chunk);
}



//Clear all the statistics
void base_cache::clear_stat(){
    hit = miss = 0; //local statistics
    delete cache_stats;
    cache_stats = new cache_stat_entry[__file_bulk+1];
}
