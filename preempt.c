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
#

void preempt_disable(void)
{
  struct sigaction signal;

  sigprocmask(SIG_BLOCK, &signal.sa_mask ,NULL);
}

void preempt_enable(void)
{
  struct sigaction signal;

  sigprocmask(SIG_UNBLOCK, &signal.sa_mask ,NULL);
}

void handler(int signum)
{
  uthread_yield();
}

// https://www.gnu.org/software/libc/manual/html_mono/libc.html#Handler-Returns
unsigned int alarm (void)
{
  struct itimerval old, new;
  new.it_interval.tv_usec = 0;
  new.it_interval.tv_sec = 0;
  new.it_value.tv_usec = 0;
  new.it_value.tv_sec = 0;
  if (setitimer (ITIMER_VIRTUAL, &new, &old) < 0)
    return 0;
  else
    return old.it_value.tv_sec;
}

void preempt_start(void)
{
  struct sigaction signal;

  signal.sa_handler = handler;
  sigaction(SIGVTALRM, &signal, NULL);
  alarm();
}

