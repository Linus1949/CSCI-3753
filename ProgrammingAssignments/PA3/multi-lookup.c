#include "multi-lookup.h"

void* request(void* inputFile)
{
    int tid = syscall(SYS_gettid);
    reqData* request_th = (reqData*) inputFile;
    critical* currentPos = request_th->address;

    pthread_mutex_t* mutX = currentPos->mutX;
    pthread_mutex_t* mutX_File = currentPos->mutX_File;
    pthread_mutex_t* mutx_Lock = currentPos->mutX_Lock;

    int firstPosition  = (request_th->data) % currentPos->my_files.size;
    int size = currentPos->my_files.size;

    pthread_cond_t* condVar = currentPos->condVar;
    pthread_cond_t* pv = currentPos->condVar_File;

    queue* myQ = currentPos->myQ;
    FILE* filePtr =  (FILE *)currentPos->my_files.filePtr[firstPosition];
    int* finished = currentPos->finished;
    int startVal = 1; 
    int serve = 0;
    
    while(1)
    {
        int currentFile;
        if(!startVal)
        {
            pthread_mutex_lock(mutX_File); // mutex lock
            // critical section
            currentFile = currentPos->my_files.next;
            filePtr =  (FILE*) currentPos->my_files.filePtr[currentFile];
            int tempVar = (currentFile + 1) % size;

            while(1)
            {
                if (tempVar == currentFile)
                {
                    break;
                }
                else if(currentPos->my_files.finished_arr[tempVar] == -1)
                {
                    currentPos->my_files.next = tempVar;
                    break;
                }
                tempVar = (tempVar + 1) % size;
            }
            pthread_mutex_unlock(mutX_File); // mutex unlock
        }
        else
        {
            startVal = 0;
            currentFile = firstPosition;
        }
        
        char buff[MAX_NAME_LENGTH];
        int temp = 0;
        while(1)
        {
            flockfile(filePtr);
            if(fgets_unlocked(buff, MAX_NAME_LENGTH, filePtr))
            {
                funlockfile(filePtr);
                pthread_mutex_lock(mutX); // mutex lock
                // critical section
                while(queue_full(myQ))
                {
                    pthread_cond_wait(pv, mutX); // waiting
                }
                buff[strlen(buff)-1] = '\0';
                void* cpyData = malloc(strlen(buff)+1);
                cpyData = (void *) strncpy(cpyData, buff, strlen(buff)+1);

                if(queue_push(myQ, cpyData) == -1)
                {
                    printf("ERROR: Can't send data to the shared array!\n");
                }
                pthread_cond_signal(condVar);
                pthread_mutex_unlock(mutX); // mutex unlock
                temp++;
            }
            else
            {
                funlockfile(filePtr);
                if(temp == 0)
                {
                    serve--;
                }
                break;
            }
        }
        serve++;

        pthread_mutex_lock(mutX_File); // locking mutex
        // critical section
        if(currentPos->my_files.finished_arr[currentFile] == -1)
        {
            currentPos->my_files.finished_arr[currentFile] = 0;
            currentPos->my_files.current++;
        }
        pthread_mutex_unlock(mutX_File); // unlocking mutex
        
        char charArr[50];
        if(currentPos->my_files.size == currentPos->my_files.current)
        {
            FILE* curFile = currentPos->service;
            flockfile(curFile);
            snprintf(charArr, sizeof(charArr), "Thread %d serviced %d files.\n", tid, serve); // writing to the output
            fputs_unlocked(charArr, curFile);
            funlockfile(curFile);

            pthread_mutex_lock(mutx_Lock); // mutex lock 
            *finished = 1; // critical section
            pthread_mutex_unlock(mutx_Lock); // mutex unlock
            pthread_cond_signal(condVar); //signal
            break;
        }
    }

    pthread_exit(NULL); // returning NULL
}

void* resolve(void* outputFile)
{
    process* currentProcess = (process *) outputFile;
    queue* myQ = currentProcess->myQ;

    pthread_mutex_t* mutX = currentProcess->mutX; // mutex
    pthread_mutex_t* mutx_Lock = currentProcess->mutX_Lock; // mutex
    pthread_cond_t* condVar = currentProcess->condVar; // condition variable
    pthread_cond_t* condVar_File = currentProcess->condVar_File; // condition variable

    FILE* filePtr = currentProcess->fileOutput;
    int* finished = currentProcess->finished;

    while(1)
    {
        pthread_mutex_lock(mutX); // mutex lock
        while(queue_empty(myQ) && !(*finished))
        {
            pthread_cond_wait(condVar, mutX); // waiting
        }

        pthread_mutex_lock(mutx_Lock); // mutex lock 
        if((*finished) && queue_empty(myQ))
        {
            pthread_mutex_unlock(mutx_Lock); // mutex unlock
            pthread_cond_signal(condVar); // signal
            pthread_mutex_unlock(mutX); // mutex unlock
            break;
        }
        pthread_mutex_unlock(mutx_Lock); // mutex unlock
        // critical section
        char* Qdata = (char*) queue_pop(myQ);

        pthread_cond_broadcast(condVar_File);
        pthread_mutex_unlock(mutX);

        char ip_str[INET6_ADDRSTRLEN];
        char my_data[strlen(Qdata) + 1];
        int ip_res = dnslookup(Qdata, ip_str, sizeof(ip_str)); // dnslookup from util
        
        strncpy(my_data, Qdata, strlen(Qdata) + 1);
        free(Qdata);
        strcat(my_data, ",");

        if (ip_res != -1)
        {
            char charOut[125];
            strncpy(charOut, my_data, strlen(my_data)+1);
            strncat(charOut, ip_str, strlen(ip_str));
            strcat(charOut, "\n");

            flockfile(filePtr);
            fputs_unlocked(charOut, filePtr);
            funlockfile(filePtr);
        }
        else
        {
            fprintf(stderr, "ERROR: Bogus Hostname!\n");
            strncat(my_data, "\n", 2);

            flockfile(filePtr);
            fputs_unlocked(my_data, filePtr);

            funlockfile(filePtr);
        }
        // signal
    }

    pthread_exit(NULL); // returns NULL
}

int main(int argc, char* argv[])
{
    struct timeval startTime;
    struct timeval endTime;
    gettimeofday(&startTime, NULL); // for the runtime
    
    if(argc > 15)
    {
        fprintf(stderr, "ERROR: Input files > 15!\n");
        exit(1);
    }
    else if(argc < 6)
    {
        fprintf(stderr, "ERROR: Input files < 6!\n");
        exit(1); 
    }
    int argv1Req = atoi(argv[1]);
    int argv2Res = atoi(argv[2]);
    
    pthread_t request_thread[argv1Req];
    pthread_t resolve_thread[argv2Res];

    if(access(argv[3], F_OK) == -1)
    {
        fprintf(stderr, "ERROR: Service file!\n");
        exit(1);
    }
    if(access(argv[4], F_OK) == -1)
    {
        fprintf(stderr, "ERROR: Requester file!\n");
        exit(1);
    }

    int *myArray = (int *) malloc(sizeof(int) * argc-5);

    // iterating through the files 
    int numFiles = 0;
    for(int i = 5; i < argc; i++)
    {
        if(access(argv[i], F_OK) == -1)
        {
            fprintf(stderr, "Skipping a file!\n");
            fprintf(stderr, "%s.\n", argv[i]);
            myArray[i-5] = -1;
        }
        else
        {
            FILE* inputFile = fopen(argv[i], "r"); // opening to read

            if(inputFile == NULL)
            {
                fprintf(stderr, "Skipping an existing file!\n");
                myArray[i-5] = -1;
            }
            else
            {
                numFiles++;
                myArray[i-5] = i;
                fclose(inputFile);
            }
        }
    }

    int* files = (int *)malloc(sizeof(int)* numFiles);
    void** filePtr = malloc(sizeof(FILE*)* numFiles);

    int i;
    int tmp = 0;
    for(i = 0; i < numFiles; i++)
    {
        files[i] = -1;
    }
    for(i = 0; i < argc - 5;i++)
    {
        if(myArray[i] != -1)
        {
            FILE* fp = fopen(argv[i + 5], "r"); //
            filePtr[tmp] = (void *)fp; // casting to void *
            tmp++;
        }
    }

    int finished = 0;
    critical helper;
    FILE* services = fopen(argv[3], "w"); // opening to write

    helper.service = services;
    helper.my_files.finished_arr = files;
    helper.my_files.filePtr = filePtr;
    helper.my_files.next = (((argv1Req-1) % numFiles) +1) % numFiles;
    helper.my_files.current = 0;
    helper.my_files.size = numFiles;
    helper.finished = &finished;

    queue myQ;
    pthread_mutex_t mutX; // mutex
    pthread_mutex_t mutx_Lock; // mutex
    pthread_mutex_t mutX_File; // mutex
    pthread_cond_t condVar; // condition variable
    pthread_cond_t condV; // condition variable

    helper.condVar_File = &condV;
    helper.condVar = &condVar;
    helper.mutX = &mutX;
    helper.myQ = &myQ;
    helper.mutX_Lock = &mutX_File;
    helper.mutX_File = &mutx_Lock;

    pthread_mutex_init(&mutX, NULL); // mutex
    pthread_mutex_init(&mutx_Lock, NULL); // mutex
    pthread_mutex_init(&mutX_File, NULL); // mutex
    pthread_cond_init(&condVar, NULL); // condition variable
    pthread_cond_init(&condV, NULL); // condition variable

    FILE *writeOut = fopen(argv[4], "w"); // output write
    process myProcess;
    myProcess.finished = &finished;
    myProcess.condVar_File = &condV;
    myProcess.condVar = &condVar;
    myProcess.mutX = &mutX;
    myProcess.myQ = &myQ;
    myProcess.mutX_Lock = &mutX_File;
    myProcess.fileOutput = writeOut;

    if(queue_init(&myQ, 255) == -1)
    {
        fprintf(stdout, "Failed to create queue, exiting now.\n");
        exit(1);
    }
    reqData req_arr[argv1Req];

    // creating threads
    int j;
    for(j = 0; j < argv1Req; j++)
    {
        req_arr[j].address = &helper;
        req_arr[j].data = j;
        pthread_create(&request_thread[j], NULL, request, (void *) &req_arr[j]);
    }
    int k;
    for(k = 0; k < argv2Res; k++)
    {
        pthread_create(&resolve_thread[k], NULL, resolve, (void *) &myProcess);
    }
    for(j = 0; j < argv1Req; j++)
    {
        pthread_join(request_thread[j], NULL);
    }
    for(k = 0; k < argv2Res; k++)
    {
        pthread_join(resolve_thread[k], NULL);
    }

    // stop time and print
    gettimeofday(&endTime, NULL);
    printf("Total runtime: %ld microseconds.\n", (((endTime.tv_sec * 1000000) + endTime.tv_usec) - ((startTime.tv_sec * 1000000) + startTime.tv_usec)));
    
    // closing files
    fclose(services);
    fclose(writeOut);
    for(int i = 0; i < numFiles; i++)
    {
        fclose((FILE *)filePtr[i]);
    }
    // freeing memory
    queue_free(&myQ);
    free(myArray);
    free(files);
    free(filePtr);

    return 0; 
}