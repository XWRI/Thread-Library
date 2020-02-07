#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define TEST_ASSERT(assert)			\
do {						\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {				\
		printf("PASS\n");		\
	} else	{				\
		printf("FAIL\n");		\
		exit(1);			\
	}					\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Enqueue/Dequeue complex */
void test_queue_complex(void)
{
	int data_1 = 3;
	char data_2 = 's';
	int *data_3 = malloc(5 * sizeof(int));
	struct {
			int val;
			char label;
	} data_4 = { 10, 't'};
	char *data_5 = "p2 is about threads";


	int *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_complex ***\n");

	q = queue_create();
	queue_enqueue(q, &data_1);
	queue_enqueue(q, &data_2);
	queue_enqueue(q, data_3);
	queue_enqueue(q, &data_4);
	queue_enqueue(q, data_5);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data_1);
}

/* Delete simple */
void test_queue_delete_simple(void)
{
	int data_1 = 3;
	int data_2 = 4;
	int data_3 = 5;
	int data_4 = 6;
	int data_5 = 7;

	int *ptr_2 = &data_2;
	int *ptr_1;
	int *ptr_3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data_1);
	queue_enqueue(q, &data_2);
	queue_enqueue(q, &data_3);
	queue_enqueue(q, &data_4);
	queue_enqueue(q, &data_5);
	TEST_ASSERT(queue_delete(q, (void*)ptr_2) == 0);
	queue_dequeue(q, (void**)&ptr_1);
	TEST_ASSERT(ptr_1 == &data_1);
	queue_dequeue(q, (void**)&ptr_3);
	TEST_ASSERT(ptr_3 == &data_3);
}

/* Delete complex */
void test_queue_delete_complex(void)
{
	int data_1 = 3;
	char data_2 = 's';
	int *data_3 = malloc(5 * sizeof(int));
	struct {
			int val;
			char label;
	} data_4 = { 10, 't'};
	char *data_5 = "p2 is about threads";


	int *ptr = data_3;
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete_complex ***\n");

	q = queue_create();
	queue_enqueue(q, &data_1);
	queue_enqueue(q, &data_2);
	queue_enqueue(q, data_3);
	queue_enqueue(q, &data_4);
	queue_enqueue(q, data_5);
	TEST_ASSERT(queue_delete(q, (void*)ptr) == 0);
}

/* Length */
void test_length(void)
{
	int data_1 = 3;
	int data_2 = 4;
	int data_3 = 5;
	int data_4 = 6;
	int data_5 = 7;

	queue_t q;

	fprintf(stderr, "*** TEST queue_length***\n");

	q = queue_create();
	queue_enqueue(q, &data_1);
	queue_enqueue(q, &data_2);
	queue_enqueue(q, &data_3);
	queue_enqueue(q, &data_4);
	queue_enqueue(q, &data_5);
	TEST_ASSERT(queue_length(q) == 5);
}

int main(void)
{
	test_create();
	test_queue_simple();
	test_queue_complex();
	test_queue_delete_simple();
	test_queue_delete_complex();
	test_length();

	return 0;
}
