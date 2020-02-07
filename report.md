
<h3> ECS 150 Programming Assignment 2</h3>
<h4>Xiaowei Min & Wenbai Zhang</h4>
<h4>2/6/2020</h4>

<p> For this assignment, we followed the suggested phase to complete the task
</p>

<strong><em>Queue implementation</em></strong>

<p>We used a doubly-linked list to implement the queue data structure. Initially
, we experimented with singly-linked list. However, we decide that a doubly
-linked list will better support the function of delete. Moreover, we created a
struct queue_node to contain the information about queue node value and the
previous and next connecting to it. The variable size kept track of the size of
the queue, which makes queue_length() O(1), saving the trouble to iterate
through the linked list. The implementation of queue_enqueue() and
queue_dequeue() are pretty straightforward. Since it is a FIFO strucutre, we
just need to append to the tail node or popping the head node, which are both
O(1) as well. queue_delete() and queue_iterate() takes O(n) since we need to
iterate through the linked-list.
</p>

<strong><em>uthread API</em></strong>

<p> First of all, we created a struct to hold the all the information related
to a thread, including the tid, context, stack, the blocking thread, the
function to be executed and the argument to be passed in, the return value and
whether it is already being joined. We used this struct to keep track of the
status of each thread. Moreover, we created four queues recording the running
thread(since there is only one thread, the length is always smaller than one),
the ready threads, the dead threads(zombie) and the blocked threads.
</p>

<p> For the uthread_create(), we created a separate function uthread_init() to
initialize the library, set up the queues and the main thread. We also enqueue
the main thread into the running queue. Afterwards, we initialize the first
thread with the argument @func and @arg, and we put the first thread into the
ready queue, waiting to be executed.
</p>

<p> For uthread_yield(), we took out the currently active and running thread
from the running queue and took out the first thread waiting to be executed in
the ready queue. Then, we input the first waiting thread into the running queue.
Afterwards, we do a context switch to yield for the other threads to execute.
</p>

<p> For uthread_exit(), we still first took out the currently active and running
thread from the running queue, and assign the input return val to the currently
running thread. Then, from that thread, we use queue_iterate() to find the
thread that the currently running thread is blocking if there is
any. If there is, we took out that blocked thread and put it inside the ready
queue. We also let the blocked thread collects the return value. After we
unblocked the thread, we get the next thread to be execute from the ready queue
and input it to the running queue and do a context switch between the previous
thread and the new running thread.
</p>

<p>For uthread_self(), it has the most straightforward implementation. We just
get the currently running thread from the running queue and return its tid.
However, one thing to note is that we need to enqueue the currently running
thread back to the running queue after we dequeue it.
</p>

<p> For uthread_join(), we implemented it based on the instruction given in the
assignment. Firstly, we looked for the cases where the function would return -1.
We check all the conditions before continuing with the actual joining function.
To check the conditions, we created two helper functions: find_joined() and
find_tid() to realize the function. Together with queue_iterate(), we checked
if the give tid is already being joined and whether it can be found in tne
queue. After we have ruled out all the possibilities that it should return -1,
we went on to see which situation does the current one falls into; whether the
given @tid is already dead or it is still an active thread. The dead situation
is simpler(the thread is found in the zombie queue), as we only need to collect
the return value and free the space occupied by that thread. But for the case
that the thread is still active(the thread is found in the ready queue), it is
more complex. We first need to take out that thread from the ready queue. This
is enabled by iterating through the queue. Once we have successfully dequeued
the thread from the ready queue, we put it inside the running queue. After a
context switch, we store its return val to the input address.
</p>

<strong><em>Preempt</em></strong>

<p>For this part, we looked at the resources given in the instruction. We used
sigaction instead of signal. We created the handler() that calls uthread_yield()
as the signal handler. We looked at an example of alarm() on the resource
website and used its content to configure the timer.
</p>

<strong><em>Testing</em></strong>

<p> For the testing of queue implementation, we added more test cases in
addition to the given ones. We tested for delete and iterate and length.
Moreover, in addition to the simple type of input, which is int, we modify the
data type to be of various type and tested for enqueue, dequeue and delete as
well.
</p>

<p> For the testing of output redirection, we tried with incomplete command,
including missing output file, missing writing instructions, invalid output
file and multiple commands after the symbol '>'. We tested for the possible
errors and then, for the correct input to see if the correct output is reached.
</p>

<p> For the uthread API, we first used the given hello and yield to check if the
program is working. Then we created more tests to cover the other situations.
We tried to join different threads and in different sequence to test if the
program produce the right result.
</p>

<p> For the preemption, we have different modes to test the program. The initial
one is the default. The second one is when we enabled preempt. The third one is
when we call preempt_disable(). So we set a fixed time interval different things
to be printed out when preempt_enable() and preempt_disable() is called.
In a duration, we alternate between the two to see if they are working
correctly.
</p>

<p> Overall, this assignment requires a good understanding of how the thread
works. It took us some time to understand the exact ordering of the thread
inside the library. Once we understood it, it is easier to implement. Moreover,
we find the preempt a bit hard too since it is our first time learning about it.
We used some of the examples in the document to help with the coding. 
</p>
