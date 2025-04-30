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
 
// /* 
//  * initCache - Allocate memory, write 0's for valid and tag and LRU
//  * also computes the set_index_mask
//  * FILL THIS FUNCTION IN
//  */

typedef struct cacheLine{
    int validBit;
    int tag;
    int LRUTrack;
}cacheLineT;

typedef cacheLineT* cacheSetT;
typedef cacheSetT* cacheT;

cacheT cache;

void initCache() {
    cache = malloc(S * sizeof (cacheSetT));
    if(cache == NULL) {
        perror("ERROR at cache == null");
        exit(1);
    }

    for(int i = 0; i < S; i++){
        cache[i] = malloc(E * sizeof (cacheLineT));
        if(cache[i] == NULL) {
            free(cache);
            perror("ERROR at cache[i] == null");   
            exit(1);         
        }

        for(int j = 0; j < E; j++){
            cache[i][j].validBit = 0;
            cache[i][j].tag = 0;
            cache[i][j].LRUTrack = 0;
        }
    }
    return;
}


// /* 
//  * freeCache - free allocated memory
//  * FILL THIS FUNCTION IN
//  */

void freeCache() {
    for(int i = 0; i < S; i ++){
        free(cache[i]);
    }
    free(cache);
    return;
}

/*
 * replayTrace - replays the given trace file against the cache 
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
   char operation[2];
   int address;
   int size;
   int count = 0;
    /* open file for reading*/
    fp = fopen(filename, "r");
    if(fp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    //for the tag we need the first some # of bits
    int tagMask = -1 << (s + b);
   // printf("%d tagmask \n", tagMask); 
    int setMask = (-1 << b) & ~(tagMask);
    //printf("%d setMask \n", setMask);

    /*fgets goes through a line of the file at a time. */
    while( fgets (str, 256, fp)!=NULL ) {
        
        /*
            scan line of file for the operation, address, and size of the trace instruction
            IMPORTANT: here "I" is filtered out because the %*[ ] requires a leading space 
                       for the line to be scanned
         */
        if (sscanf(str, "%*[ ] %s %d, %d", operation, &address, &size)){
            count++;
        }
        printf("%d %s %d %d \n", count, operation, address, size);

        //step 1: parse the address
        int tag = address & tagMask;
        tag = tag >> (s+b);

        int set = address & setMask;
        set = set >> b;
        //printf("address: %x \n", address);
        // printf("tag: %d set: %d \n", tag, set);
     
        int hit = 0;
        int cacheSpaceTrue = 0;
        int emptyLine = -1;

        int highestLRU = -1;
        int locHighestLRU = -1;


        /*
            step 2: check if its a hit or miss. For loop through cache[set] looking for a matching tag. 
            Then have an if statement about if the valid bit is 1. 
        */
        for(int i = 0; i<E; i++){ 
            // printf("validBit %d \n", cache[set][i].validBit);

            if(cache[set][i].validBit == 1) {
                if(cache[set][i].tag == tag) {
                    //printf("HIT 1\n");
                    hit_count ++;
                    
                    hit = 1;
                } 

                else if(cache[set][i].LRUTrack > highestLRU){
                    //printf("find higher LRU \n");
                    highestLRU = cache[set][i].LRUTrack;
                    locHighestLRU = i;
                }
            }

            else{
                cacheSpaceTrue = 1;
                emptyLine = i;
            }
        
        //step 3: check if it needs to evict if its a miss if its an L or M / put in the thing
        }
        if(hit == 0){
            //printf("MISS \n");
            miss_count++;

            if(cacheSpaceTrue != 0){ //this is if there is a spot in the cache with 0 valid bit
                for(int i = 0; i < E; i++){
                    if(cache[set][i].validBit == 1){
                        cache[set][i].LRUTrack++;
                    }
                }
            for(int i = 0; i < E; i++){
                if(cache[set][i].validBit == 1){
                    cache[set][i].LRUTrack++;
                }
            }
            if(cacheSpaceTrue == 1){ //this is if there is a spot in the cache with 0 valid bit
                //printf("space in cache True 2 \n");
                cache[set][emptyLine].validBit = 1;
                cache[set][emptyLine].tag = tag;
                cache[set][emptyLine].LRUTrack = 0;
            }
            else{ //if everything is full
                //printf("eviction necessary \n");
                eviction_count++;
                cache[set][locHighestLRU].tag = tag;
                cache[set][locHighestLRU].LRUTrack = 0;
            }
        }
        //printf("OPERATION %c \n", operation[0]);
        if(operation[0] == 'M') { 
            hit_count++;
        }
    }

    
    
    }
    fclose(fp); //Problems: cache doesn't univesally update. Also, the free is fucked up and shuts the program down. 
}

int main(int argc, char* argv[]) {
    char c;
    // setting defaults for testing
    verbosity = 1;
    s = 4;
    E = 1;
    b = 4;
    trace_file = "traces/yi.trace";
    
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


    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    } 

    /* Compute S, E and B from command line args */
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);
 
    /* Initialize cache */
    initCache();

    /* Replay trace file */
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}

