#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
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
};

typedef struct uthread* uthread_job_t;

bool initialize = false; // if the thread library is initialized
uthread_t init_tid = 0;  // initial number for the thread identifier
queue_t ready;
queue_t running;
queue_t zombie;

void uthread_yield(void)
{
    if(queue_length(ready) == 0) return;

    uthread_job_t new_thread = (uthread_job_t)malloc(sizeof(uthread));
    uthread_job_t running_thread = (uthread_job_t)malloc(sizeof(uthread));
    queue_dequeue(ready, (void**)(new_thread));
    queue_dequeue(running, (void**)(running_thread));
    queue_enqueue(running, (void*)new_thread);

    queue_enqueue(ready,(void*)running_thread);
    uthread_ctx_switch(&running_thread->context, &new_thread->context);
}

uthread_t uthread_self(void)
{
  if(queue_length(running) == 0) return 0;

  uthread_job_t cur_thread = (uthread_job_t)malloc(sizeof(uthread));
  queue_dequeue(running, (void**)cur_thread);
  queue_enqueue(running, (void*)cur_thread);
  return cur_thread->tid;
}

// helper function to initialize the uthread library
int uthread_init()
{
    ready = queue_create();
    running = queue_create();
    zombie = queue_create();
    initialize = true;

    uthread_job_t cur_thread = (uthread_job_t)malloc(sizeof(uthread));

    if(cur_thread == NULL) return -1;

    cur_thread->tid =  init_tid++;
    cur_thread->stack = uthread_ctx_alloc_stack();

    queue_enqueue(running, cur_thread);

    return 0;
}

int uthread_create(uthread_func_t func, void *arg)
{
	  if(!initialize)
    {
       uthread_init();
    }

    uthread_job_t cur_thread = (uthread_job_t)malloc(sizeof(uthread));

    // In case of memory allocation failure
    if(cur_thread == NULL) return -1;

    // Sets the properties of the current thread
    cur_thread->tid = init_tid++;
    cur_thread->funct = func;
    cur_thread->arg = arg;
    cur_thread->stack = uthread_ctx_alloc_stack();

    // In case of memory failure
    if(cur_thread->stack == NULL) return -1;

    if(uthread_ctx_init(cur_thread->context, cur_thread->stack,
        cur_thread->funct, cur_thread->arg) != 0)
    {
      return -1;
    }
    queue_enqueue(ready, (void*)cur_thread);
    return cur_thread->tid;
}

void uthread_exit(int retval)
{
	/* TODO Phase 2 */
}

int uthread_join(uthread_t tid, int *retval)
{
	/* TODO Phase 2 */
	/* TODO Phase 3 */
}
