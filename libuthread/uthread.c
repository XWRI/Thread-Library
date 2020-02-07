#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <limits.h>
#include <ucontext.h>

#include "context.h"
// #include "preempt.h"
#include "queue.h"
#include "uthread.h"

struct uthread
{
  uthread_t tid;
  uthread_func_t funct;
  void* arg;
  void* stack;
  uthread_ctx_t context;
  int ret_val;
  bool joined;
  uthread_t blocking_thread;
};

typedef struct uthread* uthread_job_t;

bool initialize = false; // if the thread library is initialized
uthread_t init_tid = 0;  // initial number for the thread identifier
queue_t ready;
queue_t running;
queue_t zombie;
queue_t blocked;

// helper function to find the uthread with the specific tid
int find_tid(void *data, void *arg) {
  uthread_job_t thread = (uthread_job_t)data;
  if(thread->tid == *(uthread_t*)arg) return 1;
  return 0;
}

// helper function to see if the joining thread is already being joined
int find_joined(void *data, void *arg) {
  uthread_job_t thread = (uthread_job_t)data;
  if(thread->tid == *(uthread_t*)arg && thread->joined == true)
  {
    printf("%d\n", thread->joined == true);
    return 1;
  }
  return 0;
}

void uthread_yield(void)
{
  if(queue_length(ready) == 0) return;

  uthread_job_t new_thread = (uthread_job_t)malloc(sizeof(struct uthread));
  uthread_job_t running_thread = (uthread_job_t)malloc(sizeof(struct uthread));
  queue_dequeue(ready, (void**)(&new_thread));
  queue_dequeue(running, (void**)(&running_thread));
  queue_enqueue(running, (void*)new_thread);
  queue_enqueue(ready,(void*)running_thread);
  uthread_ctx_switch(&running_thread->context, &new_thread->context);
}

uthread_t uthread_self(void)
{
  if(queue_length(running) == 0) return 0;

  uthread_job_t cur_thread = NULL;
  queue_dequeue(running, (void**)(&cur_thread));
  queue_enqueue(running, (void*)cur_thread);
  return cur_thread->tid;
}

// helper function to initialize the uthread library
int uthread_init()
{
  ready = queue_create();
  running = queue_create();
  zombie = queue_create();
  blocked = queue_create();
  initialize = true;

  uthread_job_t cur_thread = (uthread_job_t)malloc(sizeof(struct uthread));

  if(cur_thread == NULL) return -1;

  cur_thread->tid =  init_tid++;
  cur_thread->stack = uthread_ctx_alloc_stack();
  cur_thread->joined = false;
  cur_thread->blocking_thread = -1;

  queue_enqueue(running, (void*)cur_thread);

  return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
  if(!initialize)
  {
     uthread_init();
  }

  uthread_job_t cur_thread = (uthread_job_t)malloc(sizeof(struct uthread));

  // In case of memory allocation failure
  if(cur_thread == NULL) return -1;

  // Sets the properties of the current thread
  cur_thread->tid = init_tid++;
  cur_thread->funct = func;
  cur_thread->arg = arg;
  cur_thread->stack = uthread_ctx_alloc_stack();
  cur_thread->joined = false;
  cur_thread->blocking_thread = -1;

  // In case of memory failure
  if(cur_thread->stack == NULL) return -1;

  if(uthread_ctx_init(&cur_thread->context, cur_thread->stack,
    cur_thread->funct, cur_thread->arg) != 0)
  {
    return -1;
  }
  queue_enqueue(ready, (void*)cur_thread);
  return cur_thread->tid;
}

void uthread_exit(int retval)
{
  uthread_job_t running_thread = (uthread_job_t)malloc(sizeof(struct uthread));
  queue_dequeue(running, (void**)(&running_thread));
  running_thread->ret_val = retval;
  queue_enqueue(zombie, (void*)running_thread);

 // need to find the threads that the exiting thread is blocking and add it
 // to ready queue_enqueue
 uthread_job_t blocked_thread = NULL;
 queue_iterate(blocked, find_tid, (void*)&running_thread->blocking_thread,
   (void**)&blocked_thread);

 if(blocked_thread != NULL)
 {
   blocked_thread->ret_val = retval;
   queue_delete(blocked, (void*)blocked_thread);
   queue_enqueue(ready, (void*)blocked_thread);
   running_thread->blocking_thread = -1;
 }
 // do a context switch
 uthread_job_t new_running_thread = (uthread_job_t)malloc(sizeof(struct uthread));
 queue_dequeue(ready, (void**)(&new_running_thread));
 queue_enqueue(running, (void*)new_running_thread);
 uthread_ctx_switch(&running_thread->context, &new_running_thread->context);
}

int uthread_join(uthread_t tid, int *retval)
{
  // if the joining thread is the main thread
  if(tid == 0) return -1;

  // if the @tid is the TID of the calling thread
  uthread_job_t parent_thread = (uthread_job_t)malloc(sizeof(struct uthread));
  queue_dequeue(running, (void**)(&parent_thread));

  if(parent_thread->tid == tid) return -1;
  //
  uthread_job_t ready_child = NULL;
  uthread_job_t zombie_child = NULL;
  queue_iterate(ready, find_joined, (void*)&tid, (void**)&ready_child);
  queue_iterate(zombie, find_joined, (void*)&tid, (void**)&zombie_child);

  //if the @tid is already being joined
  if(ready_child != NULL || zombie_child != NULL) return -1;

  queue_iterate(ready, find_tid, (void*)&tid, (void**)&ready_child);

  // if T2(child) is still an active thread
  if(ready_child != NULL)
  {
    queue_enqueue(blocked, (void*)parent_thread);
    ready_child->blocking_thread = parent_thread->tid;

    uthread_job_t thread = (uthread_job_t)malloc(sizeof(struct uthread));
    while(true)
    {
      queue_dequeue(ready, (void**)&thread);

      // found the thread with @tid
      if(thread == ready_child)
      {
        ready_child->joined = true;
        queue_enqueue(running, (void*)ready_child);
        uthread_ctx_switch(&parent_thread->context, &ready_child->context);
        retval = &(ready_child->ret_val);
        break;
      }
      else
      {
        queue_enqueue(ready, (void*)thread);
      }
    }
  }

  queue_iterate(zombie, find_tid, (void*)&tid, (void**)&zombie_child);
  // if T2 is already dead
  if(zombie_child != NULL)
  {
    retval = &zombie_child->ret_val;
    queue_delete(zombie, (void*)zombie_child);
    uthread_ctx_destroy_stack(zombie_child->stack);
    free(zombie_child);
  }
  // if uthread @tid cannot be found
  else
  {
    return -1;
  }
  return 0;
}
