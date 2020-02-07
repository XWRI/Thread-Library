#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue_node
{
  void* data;
  struct queue_node* next;
  struct queue_node* prev;
};

typedef struct queue_node* queue_node_t;

struct queue
{
  struct queue_node* front;
  struct queue_node* back;
  int size;
};

// creates a new queue_node and set its data
struct queue_node* create_queue_node(void* data)
{
  queue_node_t node = (queue_node_t)malloc(sizeof(struct queue_node));
  node->data = data;
  node->next = NULL;
  node->prev = NULL;
  return node;
}

// creates the queue and initialize the front and back and size
queue_t queue_create(void)
{
  queue_t cur_queue = (queue_t)malloc(sizeof(struct queue));
  cur_queue->front = NULL;
  cur_queue->back = NULL;
  cur_queue->size = 0;
  return cur_queue;
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

// creates a new queue_node with the data provided and put it at the end of the
// queue
int queue_enqueue(queue_t queue, void *data)
{
  queue_node_t node = create_queue_node(data);
  if(node == NULL || data == NULL || queue == NULL) return -1;

  if(queue->front == NULL || queue->back == NULL)
  {
    queue->front = node;
  }
  else
  {
    queue_node_t temp_node = queue->back;
    node->prev = temp_node;
    temp_node->next = node;
  }
  queue->back = node;
  queue->size += 1;
  return 0;
}

// dequeues a queue_node from the front of the queue and stores the data item to
// @void** data
int queue_dequeue(queue_t queue, void **data)
{
  if(queue == NULL || queue->front == NULL || queue->front->data == NULL)
  {
    return -1;
  }

  queue_node_t popNode = queue->front;
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

// iterates through the queue and deletes the first queue_node that contains
// @void *data
int queue_delete(queue_t queue, void *data)
{
  if(data == NULL || queue == NULL || queue->front == NULL) return -1;

  queue_node_t cur_node = queue->front;
  while(cur_node != NULL )
  {
    if(cur_node->data == data)
    {
      if(queue->front->data == data)
      {
        queue_node_t temp_node = cur_node->next;
        free(queue->front);
        queue->front = temp_node;
        temp_node = NULL;
      }
      else
      {
        cur_node->prev->next = cur_node->next;
        cur_node->next->prev = cur_node->prev;
        cur_node->prev= NULL;
        cur_node->next = NULL;
        free(cur_node);
      }
      queue->size -= 1;
      return 0;
    }
    cur_node = cur_node->next;
  }
  return -1;  //did not find the node
}

// iterates through the queue and calls function @func with arguments @arg on
// each data item
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
  if(queue == NULL || queue->front == NULL || func == NULL) return -1;

  queue_node_t cur_node = queue->front;
  while(cur_node != NULL)
  {
    if(func(cur_node->data, arg) == 1)
    {
      if(data != NULL) *data = cur_node->data;
      return 0;
    }
    cur_node = cur_node->next;
  }
  return 0;
}

int queue_length(queue_t queue)
{
  if(queue == NULL || queue->front == NULL) return -1;

  return queue->size;
}

