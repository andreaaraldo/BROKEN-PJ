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
#include "ccnsim.h"
#include "content_distribution.h"
#include "zipf.h"
#include <algorithm>

//<aa>
#include <error_handling.h>
//</aa>

Register_Class(content_distribution);


vector<file> content_distribution::catalog;
zipf_distribution  content_distribution::zipf;

name_t  content_distribution::stabilization_bulk = 0;
name_t  content_distribution::perfile_bulk = 0;
name_t  content_distribution::cut_off = 0;
int  *content_distribution::repositories = 0;
int  *content_distribution::clients = 0;
int  *content_distribution::total_replicas_p;



//Initialize the catalog, the repository, and distributes contents among them.
void content_distribution::initialize(){

    double coff = par("cut_off");

    nodes = getAncestorPar("n");
    num_repos = getAncestorPar("num_repos"); //Number of repositories (specifically ccn_node(s) which have a repository connected to them)
    num_clients = getAncestorPar ("num_clients");
    alpha = par("alpha");
    q = par ("q");
    cardF = par("objects"); //Number of files within the system
    F = par("file_size"); //Average chunk size
    degree = getAncestorPar("replicas");

	//<aa>
	// CHECK_INPUT{
		if (cardF == 0){
	        std::stringstream ermsg; 
			ermsg<<"The catalog size is 0. Are you sure you intended this. If you are sure, please "<<
				" disable this exception";
			severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}
	// }CHECK_INPUT
	//</aa>


    content_distribution::total_replicas_p = (int*) malloc ( sizeof(int) );
	*(content_distribution::total_replicas_p) = 0;


    catalog.resize(cardF+1); // initialize content catalog


    //
    //Zipf initialization
    //
    zipf = zipf_distribution(alpha,q,cardF);
    zipf.zipf_initialize();

    cut_off = zipf.value(coff);
    stabilization_bulk = zipf.value(0.9);
    perfile_bulk = zipf.value(0.5);


    char name[15];
    //
    //Repositories initialization
    //
    cStringTokenizer tokenizer(getAncestorPar("node_repos"),",");
    repositories = init_repos(tokenizer.asIntVector());

    //Useful for statitics: write out the name of each repository within the network
    for (int i = 0; i < num_repos; i++){
		sprintf(name,"repo-%d",i);
		recordScalar(name,repositories[i]);
    }

    //
    //Clients initialization
    //
    if (num_clients < 0) //If num_clients is < 0 all nodes of the network are clients
		num_clients = nodes;
    tokenizer = cStringTokenizer(getAncestorPar("node_clients"),",");
    clients = init_clients (tokenizer.asIntVector());

    //Useful for statitics: write out the name of each client within the network
    for (int i = 0; i < num_clients; i++){
		sprintf(name,"client-%d",i);
		recordScalar(name,clients[i]);
    }


	//</aa>

    //
    //Content initialization
    //
    cout<<"Start content initialization..."<<endl;
    init_content();
    cout<<"Content initialized"<<endl;

	//<aa>
	finalize_total_replica();
	//</aa>
}

//<aa>
void content_distribution::finalize_total_replica(){

	*total_replicas_p = cardF*degree;
}

#ifdef SEVERE_DEBUG
void content_distribution::verify_replica_number(){
	if (*total_replicas_p != cardF*degree)
	{
        std::stringstream ermsg; 
		ermsg<<"Ctlg size="<< cardF <<". total_replica="<<*total_replicas_p;
	    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}
}
#endif
//</aa>


/* 
 * Generate all possible combinations of binary strings of a given length with
 * a given number of bits set.
*/
vector<int> content_distribution::binary_strings(int num_ones,int len){

    vector<int> bins;
    int ones,bin;
    for (int i =1;i< (1<<len);i++){
		bin = i; //<aa> It is the binary string //</aa>
		ones = 0;
		//Count the number of ones
		while (bin){
			ones += bin & 1;
			bin >>= 1;
		}
		//If the ones are equal to the number of repositories this is a good
		//binary number
		if (ones == num_ones)
			bins.push_back(i);
    }
    return bins;

}

//<aa> Return a string of bit representing an object placement. 
// There is a 1 in the i-th position iff the object is served by the i-th repo
int content_distribution::choose_repos ( ){
	int repo_string = repo_strings[intrand(repo_strings.size())];

	#ifdef SEVERE_DEBUG
	int num_1_bits =  __builtin_popcount (repo_string); //http://stackoverflow.com/a/109069
												// Number of bits set to 1 (corresponding 
												// to the number of repositories this object was 
												// assigned to)
	if (num_1_bits != degree){
		std::stringstream ermsg; 
		ermsg<<"an object has been assigned to "<< num_1_bits <<" repos while degree="<<degree;
	    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
	}

	*total_replicas_p += num_1_bits;
	#endif

	return repo_string;
}
//</aa>

//Store information about the content:
void content_distribution::init_content()
{
	// <aa>
	// In repo_card we count how many obects each repo is storing
	// <aa>
	vector<int> repo_card(num_repos,0); 

    //As the repositories are represented as a string of bits, the function
    //binary_string is used for generating binary strings of length num_repos
    //with exactly degree ones
	//<aa> where degree is the number of replicas. Each string represents a replica
	// placement of a certain object among the repositories. Given a single string, 
	// a 1 in the i-th position means that a replica of that object is placed in the 
	// i-th repository </aa>
    repo_strings = binary_strings(degree, num_repos);

	//<aa>cardF indicates how many objects there are into the catalog</aa>
    for (int d = 1; d <= cardF; d++)
    {
    	//<aa>d is a content </aa>
		//Reset the information field of a given content
		__info(d) = 0;

		//<aa> F is the size of a file</aa>
		if (F > 1){
			//Set the file size (distributed like a geometric)
			filesize_t s = geometric( 1.0 / F ) + 1;
			__ssize ( d, s );
		}else 
			__ssize( d , 1);

		// <aa>
		vector<int> chosen_repos; 
		// </aa>

		//Set the repositories
		if (num_repos==1){
			__srepo ( d , 1 );
			// <aa> Compute the chosen_repo
			chosen_repos.push_back(0);
			// </aa>
		} else {
			// <aa> Choose a replica placement among all the possibile ones. 
			// 		repos is a replica placement </aa>				
			repo_t repos = choose_repos();
			__srepo (d ,repos);

			// <aa> Compute the chosen_repos
			repo_t repo_extracted = __repo(d);
			unsigned k = 0;
			while (repo_extracted)
			{	if (repo_extracted & 1) 
					chosen_repos.push_back(k);
				repo_extracted >>= 1;
				k++;
			}
			// </aa>
		}

		// <aa> Update the repository cardinality
		for (unsigned repo_idx = 0; repo_idx < chosen_repos.size(); repo_idx++)
					repo_card[ chosen_repos[repo_idx] ] ++;
		// </aa>

    }

	// <aa> Record the repository cardinality
	for (int repo_idx = 0; repo_idx < num_repos; repo_idx++){
	    char name[15];
		sprintf(name,"repo-%d_card",repo_idx);
		recordScalar(name, repo_card[repo_idx] );
	}
    // </aa>
}
/*
* Initialize the repositories vector. This vector is composed of the
* repositories specified in the ini file.  In addition some random repositories
* are added if one wished more repositories than the fixed number specified
* (see omnet.ini for further comments).
* <aa>
* Return value:
* 	repositories[.], where repositories[i] = d means that the i-th repository is
*					 in node[d]
* </aa>
*/
int *content_distribution::init_repos(vector<int> node_repos){

    if (node_repos.size() > (unsigned) num_repos)
		error("You try to distribute too many repositories.");


	int i = 0;
	int *repositories = new int[num_repos];
	// <aa> Construct repositories array. repositories[i] will be the id of the
	// node which i-th repository is connected to. //</aa>
	while (node_repos.size() ){
			int r = node_repos[i];
			node_repos.pop_back();
			repositories[i++] = r;
	}

   	//<aa> We already assigned i repositories. Now we randomly assign the rest</aa>
    int new_rep;
    while ( i < num_repos  )
    {
		new_rep = intrand(nodes);
		if (find (repositories,repositories + i , new_rep) == repositories + i ){
			repositories[i++] = new_rep;
		}
    }
    
    //<aa>
    #ifdef SEVERE_DEBUG
		std::stringstream ss;
		ss<<"The repositories are in the following nodes";
		for (int j=0; j<i; j++)
			ss<<" "<<repositories[j];
		ss<<endl;
		debug_message(__FILE__,__LINE__, ss.str().c_str() );
    #endif
    //</aa>

    return repositories;
}



/*
* Initialize the clients vector. This vector is composed by the clients
* specified into the ini file.  In addition, some random clients are added if
* one wished more repositories than the fixed number specified (see omnet.ini
* for further comments).
*/
int *content_distribution::init_clients(vector<int> node_clients){

    if (node_clients.size() > (unsigned) num_clients)
	error("You try to distribute too much clients.");

    if (clients != 0)
	return clients;

    int *clients = new int [num_clients];

    int i = 0;
    while (node_clients.size()){
	int r = node_clients[i];
	node_clients.pop_back();
	clients[i++] = r;
    }

    int new_c;
    while ( i <  num_clients  ){
	new_c = intrand(nodes);
	//NOTE: in this way a client can be attached to a node
	//where a repository is already attached.
	if (find (clients,clients + i , new_c) == clients + i)
	    clients[i++] = new_c;
    }
    return clients;

}


/**

	//<aa>
	//Input check
	{
        std::stringstream ermsg; 
		if ( catalog_weights.size() != len ){
			ermsg<<"found "<< catalog_weights.size() <<"catalog_weights, "
				<<"while len is "<len;
		    severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
		}

		for (unsigned i = 0; i<catalog_weights.size(); i++)
			if ( catalog_weights[i] < 0 or catalog_weights[i] > 1){
				ermsg<<"The "<< i <<"-th catalog weight is "<< catalog_weights[i];
				severe_error(__FILE__,__LINE__,ermsg.str().c_str() );
			}				
	}
	//</aa>



**/
