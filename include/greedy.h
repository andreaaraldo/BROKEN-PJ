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
#include <omnetpp.h>
#include <algorithm>
#include "strategy_layer.h"
#include "ccn_interest.h"
class base_cache;
struct centry{
    base_cache *cache;
    int len;
    centry(base_cache *c, int l):cache(c),len(l){;}
    centry():cache(0),len(0){;}


};

bool operator<(const centry &a,const centry &b){
    return (a.len < b.len);
}


class greedy: public strategy_layer{
    public:
	void initialize();
	void finish();
	bool *get_decision(cMessage *in);
	bool *exploit(ccn_interest *interest);
	int nearest(vector<int>&);

    private:
	vector<centry> cfib;
};

