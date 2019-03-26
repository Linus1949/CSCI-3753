#include "util.h" //given: dnslookup function
#include "queue.h" // queue implemetation
#include <pthread.h> // threads, mutexes, cond. vars 
#include <sys/time.h> // int gettimeofday(struct timeval *tv, struct timezone *tz)
#include <unistd.h> // access function: int access(const char *path, int amode)
#include <sys/syscall.h> // get thread id


#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6 ADDRSTRLEN

char *fgets_unlocked(char *s, int n, FILE *stream);
int fputs_unlocked(const char *s, FILE *stream);

typedef struct fileStruct
{
    int * finished_arr;
    void ** filePtr;
    int next;
    int current;
    int size;
} files_s;

typedef struct args
{
    files_s my_files;
    FILE *service;
    queue* myQ;
    int * finished;

    pthread_mutex_t* mutX;
    pthread_mutex_t* mutX_File;
    pthread_cond_t* condVar;
    pthread_cond_t* condVar_File;
    pthread_mutex_t * mutX_Lock;
} critical;

typedef struct processor{
    FILE *fileOutput;
    queue* myQ;
    int * finished;

    pthread_mutex_t* mutX;
    pthread_mutex_t * mutX_Lock;
    pthread_cond_t* condVar;
    pthread_cond_t* condVar_File;
} process;

typedef struct fileData{
    critical * address;
    int data;
} reqData;

void* request(void *inputFile);

void* resolve(void *outputFile);

#endif