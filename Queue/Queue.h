#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_VERSION_MAJOR   0
#define QUEUE_VERSION_MINOR   1
#define QUEUE_VERSION_FIX     0

typedef struct
{
    char name[20];
    int maxQueueSize;
    int bufferLength;
    int readIndex;
    int writeIndex;
    int* queueMemory;

}int_Queue_t;

typedef struct
{
    char name[20];
    int maxQueueSize;
    int bufferLength;
    int readIndex;
    int writeIndex;
    float* queueMemory;
}float_Queue_t;

typedef struct
{
    char name[20];
    int maxQueueSize;
    int bufferLength;
    int readIndex;
    int writeIndex;
    char** queueMemory;
}string_Queue_t;


int obtain(string_Queue_t* queueRef, char* name, int maxQueueSize);  //Returns a reference to a queue.
int enqueue(string_Queue_t* queueRef, char* data);  //Adds an element to the back of a queue.
char* dequeue(string_Queue_t* queueRef);  //Removes an element from the front of a queue and returns the element.
//int preview();  //Returns the element at the front of the queue without removing the element from the queue.
//int getStatus();    //Returns information about the current state of a queue, such as the number of elements currently in the queue.
int nrOfElements(string_Queue_t* queueRef);
int show(string_Queue_t* queueRef);
char* flush(string_Queue_t* queueRef); //Removes all elements from a queue and returns the elements as an array.

#endif