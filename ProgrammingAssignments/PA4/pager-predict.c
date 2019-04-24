/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains a predictive pageit
 *      implmentation.
 */

#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"

void pager_predict(int*, int, int);
void setToOneA(int*, int*);
void setToOneB(int*, int*, int*);

void pageit(Pentry q[MAXPROCESSES]) 
{ 
    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int selection[MAXPROCESSES];
    static int initialized=0;
    static int prev_pg[MAXPROCESSES];
    
    /* Local vars */
    int proctmp, p_counter, my_pg, proc_val, i;

    /* initialize static vars on first run */
    if(!initialized){
	/* Init complex static vars here */
        for(proctmp = 0; proctmp < MAXPROCESSES; proctmp++){
            selection[proctmp] = -1;
            prev_pg[proctmp] = -1; 
        }
	initialized = 1;
    }
    
    /* TODO: Implement Predictive Paging */
    // fprintf(stderr, "pager-predict not yet implemented. Exiting...\n");
    // exit(EXIT_FAILURE);
    for(proc_val = 0; proc_val < MAXPROCESSES; proc_val++)
    {
        // if the proocess isn't active
        if(!q[proc_val].active)
        {
            continue; // do nothing
        }
        else // if the process is active
        {
            int pg_predict[15] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // max_pc only requires a pg num of 15
            p_counter = q[proc_val].pc;              
            my_pg = p_counter/PAGESIZE;         
            pg_predict[my_pg] = 1;

            // selecting prog based on the prev pg
            if(my_pg == 0 && prev_pg[proc_val] == 8)
            {
                selection[proc_val] = 1;
            }
            else if(my_pg == 0 && prev_pg[proc_val] == 3)
            { 
                selection[proc_val] = 4;  
            }
            else if(my_pg == 9 && prev_pg[proc_val] == 13)
            {
                selection[proc_val] = 2;  
            } 
            else if((prev_pg[proc_val] == 11 && my_pg == 0) || (prev_pg[proc_val] == 3 && my_pg == 10))
            {
                selection[proc_val] = 0;
            }
            else if(my_pg == 14)
            {
                selection[proc_val] = 3;
            }

            pager_predict(pg_predict, my_pg, selection[proc_val]); // function that makes a prediction
            
            for(i = 0; i < 15; i++)// 1911/128 < 15
            {
                if(pg_predict[i])
                {
                    // swap the page if 'tis predicted
                    if(q[proc_val].pages[i])
                    {
                       continue;
                    }
                    else
                    {
                        pagein(proc_val, i);
                    }
                    
                }
                else
                {
                    // doing a page out if the page is not predicted
                    if(q[proc_val].pages[i])
                    {
                        if(pageout(proc_val, i))
                        {
                            continue;
                        }
                        else
                        {
                            printf("*** Error! ***\n");
                            exit(0);
                        }
                        
                    }
                }
            }
            // set the previous pg
            prev_pg[proc_val] = my_pg;
        }
    }
    // no LRU
    /* advance time for next pageit iteration */
    // tick++;
} 

void pager_predict(int* predict, int my_pg, int my_prog_var)
{
    if(my_prog_var == 0) // prg_var
    {
        // my_pg
        if(my_pg == 2)
        {
            setToOneB(&predict[3], &predict[4], &predict[10]);
        }
        else if(my_pg == 3)
        {
            setToOneB(&predict[4], &predict[10], &predict[11]);
        }
        else if(my_pg == 10)
        {
            setToOneA(&predict[0], &predict[11]);
        }
        else if(my_pg == 11)
        {
            setToOneA(&predict[0], &predict[1]);
        }
        else
        {
            setToOneA(&predict[my_pg + 1], &predict[my_pg + 2]);
        }
    }
    else if(my_prog_var == 1) // prog_var
    {
        // my_pg
        if(my_pg == 7)
        {
            setToOneA(&predict[0], &predict[8]);
        }
        else if(my_pg == 8)
        {
            setToOneA(&predict[0], &predict[1]);
        }
        else
        {
            setToOneA(&predict[my_pg + 1], &predict[my_pg + 2]);
        }
    }
    else if(my_prog_var == 2) // prog_var
    {
        // my_pg
        if(my_pg == 12)
        {
            setToOneA(&predict[9], &predict[13]);
        }
        else if(my_pg == 13)
        {
            setToOneB(&predict[0], &predict[9], &predict[10]);
        }
        else
        {
            setToOneA(&predict[my_pg + 1], &predict[my_pg + 2]);
        }
    }
    else if(my_prog_var == 3) // prog_var
    {
        // my_page
        if(my_pg == 13)
        {
            setToOneA(&predict[0], &predict[14]);
        }
        else if(my_pg == 14)
        {
            setToOneA(&predict[0], &predict[1]);
        }
        else
        {
            setToOneA(&predict[my_pg+1], &predict[my_pg+2]);
        }
    }
    else if(my_prog_var == 4) // prog_var (number)
    {
        // my_page
        if(my_pg == 2)
        {
            setToOneA(&predict[0], &predict[3]);
        }
        else if(my_pg == 3)
        {
            setToOneA(&predict[0], &predict[1]);
        }
        else
        {
            setToOneA(&predict[my_pg+1], &predict[my_pg+2]);
        }
    }
    else
    {
        // if none of the prog_vars, then id the page and run all 
        if(my_pg == 2)
        {
            setToOneB(&predict[3], &predict[4], &predict[10]);
        }
        else if(my_pg == 3)
        {
            setToOneA(&predict[4], &predict[10]);
        }
        else if(my_pg == 7)
        {
            setToOneB(&predict[0], &predict[8], &predict[9]);
        }
        else if(my_pg == 8)
        {
            setToOneB(&predict[0], &predict[9], &predict[10]);
        }
        else if(my_pg == 10)
        {
            setToOneB(&predict[0], &predict[11], &predict[12]);
        }
        else if(my_pg == 11)
        {
            setToOneB(&predict[0], &predict[12], &predict[13]);
        }
        else if(my_pg == 12)
        {
            setToOneA(&predict[0], &predict[9]);
            setToOneA(&predict[13], &predict[14]);
        }
        else if(my_pg == 13)
        {
            setToOneA(&predict[0], &predict[9]);
            setToOneA(&predict[10], &predict[14]);
        }
        else
        {
            setToOneA(&predict[my_pg+1], &predict[my_pg+2]);
        }
    }
}

void setToOneA(int* numOne, int* numTwo)
{
    *numOne = 1; // setting numOne to 1
    *numTwo = 1; // setting numTwo to 1
}

void setToOneB(int* numOne, int* numTwo, int* numThree)
{
    *numOne = 1; // setting numOne to 1
    *numTwo = 1; // setting numTwo to 1
    *numThree = 1; // setting numThree to 1
}