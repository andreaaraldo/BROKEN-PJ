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
#ifndef LOCAL_STATT
#define LOCAL_STATT

#include <omnetpp.h>
#include "abstract_node.h"
#include "statistics/statistics.h"

#include <boost/dynamic_bitset.hpp>
#include <boost/unordered_map.hpp>
#include <vector>


#define END 4000
#define FULL_CHECK 2000
#define STABLE_CHECK 3000



class local_statistics : public AbstractNode{
    public:
	virtual void initialize();
	virtual void handleMessage(cMessage *);
	virtual void finish();

	void handle_timers(cMessage *);

	//Evaluation statistics
	void miss_chunk ( uint64_t);
	void hit_chunk ( uint64_t );
	void increase_data();
	void increase_interest();

	//Stabilization
	bool stable ();
	void clear_statistics();


	//Static members available to all nodes
	static boost::dynamic_bitset<> full_bit; //bit array for full caches
	static boost::dynamic_bitset<> stable_bit; //bit array for stabel caches



    private:
	int n;

	//Stabilization stuff

	vector<double> samples;


	//Node statistics
	int data;
	int interests;
	double hit;
	double miss;
        boost::unordered_map<uint32_t,double> hit_per_file;
        boost::unordered_map<uint32_t,double> miss_per_file;

	statistics *glob_stat;

	cOutVector hit_per_fileV;
    
};

#endif
