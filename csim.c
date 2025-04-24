/* 
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses 
 * and evictions.
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "cachelab.h"

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
 
/* 
 * initCache - Allocate memory, write 0's for valid and tag and LRU
 * also computes the set_index_mask
 * FILL THIS FUNCTION IN
 */
/*
struct cacheLine *createCacheLine() {
    //struct cacheLine *cLine = malloc(256);//sizeof (struct cacheLine));
    
    if (cLine == NULL) {
        return NULL;
    }

    cLine->validBit = 0;
    cLine->tag = 0;
    cLine->LRUTrack = 0; 

    return cLine;
}

struct cacheSet *createCacheSet() {
    struct cacheSet *cSet() = malloc(sizeof (struct cacheSet));
    if (cSet = NULL) {
        return -1;
    }

    cSet->lines = malloc(E * sizeof(cacheLine));

    return cSet;
}

struct cachemaker *createCache() {
    struct cachemaker *cache = malloc(sizeof (struct cachemaker));
    if (cache == NULL) {
		return -1;
	}

    cache->sets = malloc(S * sizeof(cSet));
    if(cache->sets == NULL) {
        return -1;
    }

    //here is where to do the memset? 

    return cache;
}*/

/*
void initCache() {
    return;
}
*/

/* 
 * freeCache - free allocated memory
 * FILL THIS FUNCTION IN
 */

/* void freeCache(struct cachemaker *cache) {
    free(cache->sets->lines);
    free(cache->sets);
    free(cache);
    return;
}*/

/*
 * replayTrace - replays the given trace file against the cache 
 * FILL THIS FUNCTION IN
 */
void replayTrace(char* filename) {
    FILE *fp;
    char str[256];
    /*
    -s <s>: Number of set index bits (S = 2^s is the number of sets)
    -E <E>: Associativity (number of lines per set)
    -b <b>: Number of block bits (B = 2^b is the block size)
    -t <tracefile>: Name of the valgrind trace to replay
    */
   char operation[10];
   int address;
   int size;
   int count = 0;
    /* open file for reading*/
    fp = fopen(filename, "r");
    if(fp == NULL) {
        perror("Error opening file");
        
    }

    /*fgets goes through a line of the file at a time. */
    while( fgets (str, 256, fp)!=NULL ) {
        
        
        /*scan line of file for the operation, address, and size of the trace instructino */
        sscanf(str, "%s %d, %d", operation, &address, &size);
        printf("count: %d operation: %s address: %d size: %d \n", count, operation, address, size);

        //increase count of how many instructions we've gone through
        count++;
        /*
        We can put operation, address, size, and count into a struct here
        */

    }
    
    fclose(fp);
    
}

//int main(int argc, char* argv[]) {
    /*char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }
    */

    /* Make sure that all required command line args were specified */
    /* if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    } */

    /* Compute S, E and B from command line args */
    /*S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);*/
 
    /* Initialize cache */
    //initCache();

    /* Replay trace file */
    // replayTrace(trace_file);

    /* Free allocated memory */
    //freeCache();

    /* Output the hit and miss statistics for the autograder */
    /* printSummary(hit_count, miss_count, eviction_count);
    return 0; */
//}



int main(){
    replayTrace("traces/yi.trace");
    return 0;
}