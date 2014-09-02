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
#ifndef WEIGHTEDCONTENT_DISTRIBUTION_H
#define WEIGHTEDCONTENT_DISTRIBUTION_H
#include <omnetpp.h>
#include "ccnsim.h"
#include "content_distribution.h"
#include "zipf.h"


using namespace std;


class WeightedContentDistribution : public content_distribution{
	public:
		WeightedContentDistribution() : initialized(false) {;}
		// http://stackoverflow.com/a/7863971/2110769

		virtual const vector<double> get_weights();
		virtual const double get_priceratio();
		virtual const double get_xi();
		virtual const double get_alpha();
		virtual double *init_repo_prices(); //This method ovveride father's one


		#ifdef SEVERE_DEBUG
		virtual bool isInitialized();
		#endif


    protected:
		virtual void initialize();
		virtual int choose_repos (int object_index);
		virtual void initialize_repo_popularity();
		virtual vector<int> binary_strings(int,int);
		virtual void verify_replica_number();
		virtual void finalize_total_replica();


	private:
		std::vector<double> weights;

		double* probabilities; //Probability that an object is assigne to a repo
		bool replication_admitted;
		double priceratio;
		double xi;
		double alpha;

		#ifdef SEVERE_DEBUG
		bool initialized;
		#endif
};
#endif
