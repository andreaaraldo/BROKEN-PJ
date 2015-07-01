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
#ifndef PARTITIONED_CACHE_H_
#define PARTITIONED_CACHE_H_

#include "base_cache.h"
#include "lru_cache.h"

using namespace boost;
using namespace std;

class partitioned_cache: public base_cache
{
    public:

    protected:
        void initialize();
        bool data_store(ccn_data* data_msg);
        lru_cache** subcaches;
        unordered_map<chunk_t, unsigned> quality_map;   // Map each chunk to the representation level
                                                        // which is stored of it

    private:
};
#endif
