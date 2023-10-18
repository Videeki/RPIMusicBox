#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Queue.h"


int obtain(string_Queue_t* queueRef, char* name, int maxQueueSize)
{
    queueRef->readIndex = 0;
    queueRef->writeIndex = 0;
    queueRef->bufferLength = 0;
    strcpy(queueRef->name, name);
    queueRef->maxQueueSize = maxQueueSize;
    queueRef->queueMemory = (char**)malloc(maxQueueSize * sizeof(char*));


    if(queueRef->queueMemory == NULL)
    {
        return -1;
    }

    return 0;
}

int enqueue(string_Queue_t* queueRef, char* data)
{
    if(queueRef->bufferLength == queueRef->maxQueueSize)
    {
        printf("Buffer is full!\n");
        return -1;
    }
    queueRef->queueMemory[queueRef->writeIndex] = (char*)malloc(255 * sizeof(char));
    strcpy(queueRef->queueMemory[queueRef->writeIndex], data);

    queueRef->bufferLength++;
    queueRef->writeIndex++;
    

    if(queueRef->writeIndex == queueRef->maxQueueSize)
    {
        queueRef->writeIndex = 0;
    }

    return 0;
}

char* dequeue(string_Queue_t* queueRef)
{   
    
    if(queueRef->bufferLength == 0) 
    {
        return "Buffer is empty!\n";
    }

    char *temp = queueRef->queueMemory[queueRef->readIndex];
    
    queueRef->bufferLength--;
    queueRef->readIndex++;

    if(queueRef->readIndex == queueRef->maxQueueSize)
    {
        queueRef->readIndex = 0;
    }
    
    return temp;
}

int nrOfElements(string_Queue_t* queueRef)
{
    return queueRef->bufferLength;
}

int show(string_Queue_t* queueRef)
{
    if(queueRef->bufferLength == 0) 
    {
        printf("Buffer is empty!\n");
        return -1;
    }

    int i = queueRef->readIndex;
    while(i < queueRef->bufferLength)
    {
        printf("%s ", queueRef->queueMemory[i]);
        i++;
        if(i == queueRef->maxQueueSize)
        {
            i = 0;
        }
    }
    
    printf("\n");
    return 0;
}

char* flush(string_Queue_t* queueRef)
{
    free(queueRef->queueMemory);
    return queueRef->name;
}