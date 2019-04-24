/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    int proctmp;
    int pagetmp;
    /* my local vars */
    int proc_val, p_counter, my_pg, new_pg, min_val, min_pg;

    /* initialize static vars on first run */
    if(!initialized){
	for(proctmp=0; proctmp < MAXPROCESSES; proctmp++){
	    for(pagetmp=0; pagetmp < MAXPROCPAGES; pagetmp++){
		timestamps[proctmp][pagetmp] = 0; 
	    }
	}
	initialized = 1;
    }
    
    /* TODO: Implement LRU Paging */
    //fprintf(stderr, "pager-lru not yet implemented. Exiting...\n");
    //exit(EXIT_FAILURE);

    /* Added code */

    for(proc_val = 0; proc_val < MAXPROCESSES; proc_val++) 
    { 
        if(!q[proc_val].active)
        {
            continue;
        }
        else
        {
            p_counter = q[proc_val].pc;         
            my_pg = p_counter/PAGESIZE;
            timestamps[proc_val][my_pg] = tick;
            if(!q[proc_val].pages[my_pg]) 
            {
                // check to see if swap is possible
                // using LRU implementation to swap out in min time
                if(!pagein(proc_val, my_pg)) 
                {
                    min_val = tick;
                    for(new_pg = 0; new_pg < MAXPROCPAGES; new_pg++) 
                    {
                        if(!q[proc_val].pages[new_pg]) 
                        {
                            continue;
                        }
                        else
                        {
                            if(timestamps[proc_val][new_pg] < min_val)
                            {
                                // settin lowest timestamp
                                min_val = timestamps[proc_val][new_pg];
                                min_pg = new_pg; // setting lowest page
                            }
                        }
                    }
                    // if pageout returns failure, exit
                    if(!pageout(proc_val, min_pg))
                    {
                        printf("*** Error! ***\n");
                    }
                        
                }
            }
        }
    }    

    /* my code ends */

    /* advance time for next pageit iteration */
    tick++;
} 
