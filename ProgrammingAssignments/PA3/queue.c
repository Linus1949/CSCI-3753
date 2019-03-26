
#include "queue.h"

int queue_init(queue* q, int maxSize)
{    
    q->list = malloc(sizeof(queue_data) * maxSize);
    if(!(q->list))
    {	
	    fprintf(stderr, "Error initializing a queue!");
	    return QUEUE_ERROR;
    }
    else
    {
        int i;
        for(i = 0; i < maxSize; i++)
        {
            q->list[i].data = NULL;
        }

        q->start = 0;
        q->end = 0;
        q->maxSize = maxSize;

        return maxSize;
    }
}

int queue_empty(queue* q)
{
    if((q->start == q->end) && (q->list[q->start].data == NULL))
    {
	    return 1;
    }
    else
    {
	    return 0;
    }
}

int queue_full(queue* q)
{
    if((q->start == q->end) && (q->list[q->start].data != NULL))
    {
	    return 1;
    }
    else
    {
	    return 0;
    }
}

int queue_push(queue* q, void* data)
{
    if(queue_full(q))
    {
	    return QUEUE_ERROR;
    }
    else
    {
        q->list[q->end].data = data;
        q->end = ((q->end + 1) % (q->maxSize));
        return QUEUE_SUCCESS;
    }
}

void* queue_pop(queue* q)
{
    void* popData = NULL;
    if(queue_empty(q))
    {
	    return popData;
    }
    else
    {
        popData = q->list[q->start].data;
        q->list[q->start].data = NULL;
        q->start = ((q->start + 1) % q->maxSize);

        return popData;
    }    
}

void queue_free(queue* q)
{
    while(!queue_empty(q))
    {
	    queue_pop(q);
    }
    free(q->list);
}