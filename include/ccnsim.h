#ifndef __CCNSIM_H___
#define __CCNSIM_H___

#include <vector>
#include <omnetpp.h>

//System packets
#define CCN_I 100   //ccn interest 
#define CCN_D 200   //ccn data 
#define GHOST 5    //ghost interest

//Clients timers
#define ARRIVAL 300 //arrival of a request 
#define TIMER 400   //arrival of a request 

//Statistics timers
#define FULL_CHECK 2000
#define STABLE_CHECK 3000
#define END 4000

//Typedefs
//Catalogs fields
typedef unsigned int info_t; //representation for a catalog  entry [size|repos]
typedef unsigned short filesize_t; //representation for the size part within the catalog entry
typedef unsigned short repo_t; //representation for the repository part within the catalog entry
typedef unsigned int interface_t; //representation of a PIT entry (containing interface information)

//Chunk fields
typedef unsigned long long  chunk_t; //representation for any chunk flying within the system. It represents a pair [name|number]
typedef unsigned int cnumber_t; //represents the number part of the chunk
typedef unsigned int name_t; //represents the name part of the chunk

//Useful data structure. Use that instead of cSimpleModule, when you deal with caches, strategy_layers, and core_layers
#include "client.h"
class abstract_node: public cSimpleModule{
    public:
	abstract_node():cSimpleModule(){;}

	virtual cModule *__find_sibling(std::string mod_name){
	    return getParentModule()->getModuleByRelativePath(mod_name.c_str());
	}

	virtual int __get_outer_interfaces(){
	    return getParentModule()->gateSize("face");
	}

	bool __check_client(int interface){
	    client *c;
	    bool check= false;
	    c = dynamic_cast<client *>(getParentModule()->gate("face$o",interface)->getNextGate()->getOwnerModule());
	    if (c)
		check=true;
	    return check;
	}

	virtual int getIndex(){
	    return getParentModule()->getIndex();
	}

};
//Macros
//--------------
//Chunk handling
//--------------
//Basically a chunk is a 64-bit integer composed by two parts: the chunk_number, and the chunk id
#define NUMBER_OFFSET   32
#define ID_OFFSET        0

//Bitmasks
#define CHUNK_MSK (0xFFFFFFFFUL << NUMBER_OFFSET)
#define ID_MSK    (0xFFFFFFFFUL << ID_OFFSET )

//Macros
#define __chunk(h) ( ( h & CHUNK_MSK )  >> NUMBER_OFFSET )// get chunk number
#define __id(h)    ( ( h & ID_MSK )     >> ID_OFFSET) //get chunk id

#define __schunk(h,c) h = ( (h & ~CHUNK_MSK) | ( (unsigned long long ) c  << NUMBER_OFFSET)) //set chunk number
#define __sid(h,id)   h = ( (h & ~ ID_MSK)   | ( (unsigned long long ) id << ID_OFFSET)) //set chunk id

inline chunk_t next_chunk (chunk_t c){

    cnumber_t n = __chunk(c);
    __schunk(c, (n+1) );
    return c;

}


//--------------
//Catalog handling
//--------------
//The catalog is a huge array of file entries. Within each entry is an 
//information field 32-bits long. These 32 bits are composed by:
//[file_size|repositories]
//
//
#define SIZE_OFFSET  	16
#define REPO_OFFSET 	0

//Bitmasks
#define REPO_MSK (0xFFFF << REPO_OFFSET)
#define SIZE_MSK (0xFFFF << SIZE_OFFSET)

#define __info(f) ( content_distribution::catalog[f].info) //retrieve info about the given content 

#define __size(f)  ( (__info(f) & SIZE_MSK) >> SIZE_OFFSET ) //set the size of a given file
#define __repo(f)  ( (__info(f) & REPO_MSK) >> REPO_OFFSET )

#define __ssize(f,s) ( __info(f) = (__info(f) & ~SIZE_MSK ) | s << SIZE_OFFSET )
#define __srepo(f,r) ( __info(f) = (__info(f) & ~REPO_MSK ) | r << REPO_OFFSET )

#define __file_bulk (content_distribution::perfile_bulk + 1)



//-----------
//PIT handling 
//-----------
//Each entry within a PIT contains a field who indicates through
//which interface the back-coming interest should be sent
//
#define __sface(f,b)  ( f = f | (1<<b)  ) //Set the b-th bit
#define __uface(f,b)  ( f = f & ~(1<<b) ) //Unset the b-th bit
#define __face(f,b)   ( f & (1<<b) ) //Check the b-th bit
//
//
//

//---------------------------
//Statistics utility functions
//---------------------------
//Calculate the average of a vector of elements
template <class T>
double average(std::vector<T> v){
    T s =(T) 0;
    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++)
	s += *i;
    return (double) s * 1./v.size();
}


template <class T>
double variance(std::vector<T> v){

    T s = (T) 0;
    T ss = (T) 0;
    unsigned int N = v.size();

    for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); i++){
	s += *i;
	ss += (*i)*(*i);
    }
    return (double)(1./N)* (sqrt(N*ss- s*s));

}
#endif
