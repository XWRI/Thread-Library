#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queueNode
{
    void* data;
    struct queueNode* next;
    struct queueNode* prev;
};

typedef struct queueNode* queueNode_t;

struct queue
{
    struct queueNode* front;
    struct queueNode* back;
    int size;
};

// creates a new queueNode and set its data
struct queueNode* create_queueNode(void* data)
{
    queueNode_t node = (queueNode_t)malloc(sizeof(struct queueNode));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

// creates the queue and initialize the front and back and size
queue_t queue_create(void)
{
    queue_t curQueue = (queue_t)malloc(sizeof(struct queue));
    curQueue->front = NULL;
    curQueue->back = NULL;
    curQueue->size = 0;
    return curQueue;
}

// frees the queue
int queue_destroy(queue_t queue)
{
    if(queue == NULL || queue_length(queue) != 0)
    {
        return -1;
    }
    free(queue);
    return 0;
}

// creates a new queueNode with the data provided and put it at the end of the
// queue
int queue_enqueue(queue_t queue, void *data)
{
    queueNode_t node = create_queueNode(data);
    if(node == NULL || data == NULL || queue == NULL) return -1;

    if(queue->front == NULL || queue->back == NULL)
    {
        queue->front = node;
    }
    else
    {
        queueNode_t tempNode = queue->back;
        node->prev = tempNode;
        tempNode->next = node;
    }
    queue->back = node;
    queue->size += 1;
    return 0;
}

// dequeues a queueNode from the front of the queue and stores the data item to
// @void** data
int queue_dequeue(queue_t queue, void **data)
{
    if(queue == NULL || queue->front == NULL || queue->front->data == NULL)
    {
        return -1;
    }

    queueNode_t popNode = queue->front;
    *data = popNode->data;

    if(queue->front->next == NULL)
    {
      queue->front = NULL;
      queue->back = NULL;
      queue->size = 0;
    }
    else
    {
      queue->front = queue->front->next;
      queue->front->prev = NULL;
      queue->size -= 1;
    }

    return 0;
}

// iterates through the queue and deletes the first queueNode that contains
// @void *data
int queue_delete(queue_t queue, void *data)
{
    if(data == NULL || queue == NULL || queue->front == NULL) return -1;

    queueNode_t curNode = queue->front;
    while(curNode != NULL )
    {
        if(curNode->data == data)
        {
            if(queue->front->data == data)
            {
              queueNode_t tempNode = curNode->next;
              free(queue->front);
              queue->front = tempNode;
              tempNode = NULL;
            }
            else
            {
              curNode->prev->next = curNode->next;
              curNode->next->prev = curNode->prev;
              curNode->prev= NULL;
              curNode->next = NULL;
              free(curNode);
            }
            queue->size -= 1;
            return 0;
        }
        curNode = curNode->next;
    }

    return -1;  //did not find the node
}

// iterates through the queue and calls function @func with arguments @arg on
// each data item
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    if(queue == NULL || queue->front == NULL || func == NULL) return -1;

    queueNode_t curNode = queue->front;
    while(curNode != NULL)
    {
        if(func(curNode->data, arg) == 1)
        {
          if(data != NULL) *data = curNode->data;
          return 0;
        }
        curNode = curNode->next;
    }
    return 0;
}

int queue_length(queue_t queue)
{
    if(queue == NULL || queue->front == NULL) return -1;

    return queue->size;
}
