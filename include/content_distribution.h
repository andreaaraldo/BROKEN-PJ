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
#ifndef CONTENT_DISTRIBUTION_H
#define CONTENT_DISTRIBUTION_H
#include <omnetpp.h>
#include "ccnsim.h"
#include "zipf.h"


#pragma pack(push)
#pragma pack(1)
//
//This structure is very critical in terms of space. 
//In fact, it accounts for the startup memory requirement
//of the simulator, and should be keep as small as possible.
//
//
struct file{
    info_t info;
};
#pragma pack(pop)


using namespace std;



class content_distribution : public cSimpleModule{
    protected:
		virtual void initialize();
		void handleMessage(cMessage *){;}

		//<aa> Before me, the method was private //</aa>
		virtual vector<int> binary_strings(int,int);

		//<aa>
		virtual int choose_repos();
		//</aa>


    public:
		void init_content();
		int *init_repos(vector<int>);
		int *init_clients(vector<int>);

		static vector<file> catalog;
		static zipf_distribution zipf;

		static name_t perfile_bulk;
		static name_t stabilization_bulk; 
		static name_t cut_off;
		static int  *repositories; //<aa> repositories[i] is the node which the i-th repository is attached to</aa>
		static int  *clients;


    private:
		//<aa>
		vector<int> repo_strings; //It is a temporary variable used to generate content dispacement among repos
		//</aa>
		
		//INI parameters
		int num_repos;
		int num_clients;
		int nodes;
		int degree;
		int cardF;
		int F;

		double alpha;
		double q;


};
#endif
