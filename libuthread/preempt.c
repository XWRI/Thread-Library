#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define T 10000

struct sigaction sig;

void preempt_disable(void)
{
  sigprocmask(SIG_BLOCK, &sig.sa_mask ,NULL);
}

void preempt_enable(void)
{
  sigprocmask(SIG_UNBLOCK, &sig.sa_mask ,NULL);
}

void handler(void)
{
  uthread_yield();
}

// https://www.gnu.org/software/libc/manual/html_mono/libc.html#Handler-Returns
void preempt_start(void)
{
  struct itimerval t;
  preempt_enable();
  sig.sa_handler = (__sighandler_t)handler;
  sigaction(SIGVTALRM, &sig, NULL);
  t.it_interval.tv_usec = T;
  t.it_interval.tv_sec = 0;
  t.it_value.tv_usec = T;
  t.it_value.tv_sec = 0;
  setitimer(ITIMER_VIRTUAL, &t, NULL);
}
