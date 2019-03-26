#include <stdio.h>
#include <stdlib.h>

#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_ERROR -1
#define QUEUE_SUCCESS 0

typedef struct queue_data
{
    void* data;
} queue_data;

typedef struct queue_struct
{
    queue_data* list;
    int start;
    int end;
    int maxSize;
} queue;

int queue_init(queue* q, int maxSize);

int queue_empty(queue* q);

int queue_full(queue* q);

int queue_push(queue* q, void* data);

void* queue_pop(queue* q);

void queue_free(queue* q);

#endif