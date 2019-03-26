# CSCI 3753, Programming Assignment 3

**Contact info**:
 - Satish Upadhyaya
 - saup0513@colorado.edu

**Files**:

- <b>multi-lookup.h:</b> This file contains structs and functions for multi-lookup. Function prototypes are: 

  -`void* request(void *inputFile)`
  
  -`void* resolve(void *outputFile)`
  
- <b>multi-lookup.c:</b> This file contains the code to test the DNS lookup.
- <b>queue.c:</b> This file contains the code to test the DNS lookup.
- <b>Makefile:</b>

**To compile the program**:
- Make sure you are in the right directory (directory with the Makefile)
- `make` 

**To run the Program**:
- `./multi-lookup <num_requester_threads> <num_resolver_threads> <results file> <serviced file> [<file1>....<file N>]`
- `./multi-lookup 5 5 ./output/reqlog.txt ./output/reslog.txt ./input/names1.txt ./input/names2.txt ./input/names3.txt ./input/names4.txt ./input/names5.txt`
