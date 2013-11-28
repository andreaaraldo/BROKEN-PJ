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

Register_Class(content_distribution);


vector<file> content_distribution::catalog;
zipf_distribution  content_distribution::zipf;

name_t  content_distribution::stabilization_bulk = 0;
name_t  content_distribution::perfile_bulk = 0;
name_t  content_distribution::cut_off = 0;
int  *content_distribution::repositories = 0;
int  *content_distribution::clients = 0;



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

    //Useful for statitics: write out the name of each repository within the network
    for (int i = 0; i < num_clients; i++){
	sprintf(name,"client-%d",i);
	recordScalar(name,clients[i]);
    }

    //
    //Content initialization
    //
    cout<<"Start content initialization..."<<endl;
    init_content();
    cout<<"Content initialized"<<endl;
}

/* 
 * Generate all possible combinations of binary strings of a given length with
 * a given number of bits set.
*/
vector<int> content_distribution::binary_strings(int num_ones,int len){
    vector<int> bins;
    int ones,bin;
    for (int i =1;i< (1<<len);i++){
	bin = i;
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

//Store information about the content:
void content_distribution::init_content()
{
    //As the repositories are represented as a string of bits, the function
    //binary_string is used for generating binary strings of length num_repos
    //with exactly degree ones
    vector<int> repo_strings = binary_strings(degree, num_repos);

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

		//Set the repositories
		if (num_repos==1){
			__srepo ( d , 1 );
		} else {
			repo_t repos = repo_strings[intrand(repo_strings.size())];
			__srepo (d ,repos);
		}
    }

}
/*
* Initialize the repositories vector. This vector is composed by the
* repositories specified by the ini file.  In addition some random repositories
* are added if one wished more repositories than the fixed number specified
* (see omnet.ini for further comments).
*/
int *content_distribution::init_repos(vector<int> node_repos){

    if (node_repos.size() > (unsigned) num_repos)
	error("You try to distribute too much repositories.");

    int *repositories = new int[num_repos];

    int i = 0;
    while (node_repos.size()){
	int r = node_repos[i];
	node_repos.pop_back();
	repositories[i++] = r;
    }

    int new_rep;
    while ( i < num_repos  ){
	new_rep = intrand(nodes);
	if (find (repositories,repositories + i , new_rep) == repositories + i ){
	    repositories[i++] = new_rep;
	}
    }
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
