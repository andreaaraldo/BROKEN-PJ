#ifndef DEFS_H
#define DEFS_H

//System packets
#define CCN_I 100   //ccn interest 
#define CCN_D 200   //ccn data 

//Clients timers
#define ARRIVAL 300 //arrival of a request 
#define TIMER 400   //arrival of a request 

//Statistics timers
#define FULL_CHECK 2000
#define STABLE_CHECK 3000
#define END 4000

//Typedefs
typedef uint32_t info_t; //representation for a catalog  entry [size|repos]
typedef uint16_t filesize_t; //representation for the size part within the catalog entry
typedef uint16_t repo_t; //representation for the repository part within the catalog entry
typedef uint8_t  interface_t; //representation of a PIT entry (containing interface information)

typedef uint64_t chunk_t; //representation for any chunk flying within the system. It represents a pair [name|number]
typedef uint32_t cnumber_t; //represents the number part of the chunk
typedef uint32_t name_t; //represents the name part of the chunk


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

#define __schunk(h,c) h = ( (h & ~CHUNK_MSK) | ( (unsigned long) c  << NUMBER_OFFSET)) //set chunk number
#define __sid(h,id)   h = ( (h & ~ ID_MSK)   | ( (unsigned long) id << ID_OFFSET)) //set chunk id

inline uint64_t next_chunk (uint64_t c){

    uint32_t n = __chunk(c);
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
#endif
