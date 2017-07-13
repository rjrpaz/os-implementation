// driver.C -- TA version --

// TBD
// - test that the mutex and conditions are not global

// This code uses typecast between pointers and integral types,
// which is a usual, though not widely recommended, practice.
// Also, timeouts are supposed to be long enough for synchronization;
// this may need a closer look.

#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>

#include "queue.h"

#define time_elapsed( start, end ) \
(end.tv_sec - start.tv_sec) * 1000000L + \
(end.tv_usec - start.tv_usec)

#define max2( a, b ) \
  ((a) < (b) ? (b) : (a))

  void out (char *txt)
{
  if ( NULL == txt ) return;
  fprintf (stderr, "%s", txt );
}

pthread_mutex_t me;
pthread_cond_t cond;

typedef struct {
  Queue *q, *qa;
  void *data;
} param_t;

// test 7

// tries to add an item to a full queue.
// param->data has the item to enqueue
void* test7th1 (param_t *param)
{
  param->q->enqueue (param->data);
  return (NULL);
}

// waits for a while, then deletes an item
void* test7th2 (param_t *param)
{
  struct timespec delta, abstime;
  
  delta.tv_sec = 0;
  delta.tv_nsec = 1000; // 1 usec
  delta.tv_nsec *= 1000; // 1 msec
  delta.tv_nsec *= 10; // 10 msec
  pthread_get_expiration_np (&delta, &abstime);

  pthread_mutex_lock (&me);
  pthread_cond_timedwait (&cond, &me, &abstime);
  pthread_mutex_unlock (&me);

  return param->q->dequeue (0);
}

// test 8, required

// enqueue numbers 1..param->data
void *test8th1 (param_t *param)
{
  unsigned long item, sum = 0;
  unsigned i;

  for (i = 0; i < (unsigned) param->data; ++i ) {
    item = i+1;
    sum += item;
    param->q->enqueue ((void*) item);
  }
  
  return ((void*)sum);
}

// after a while, dequeue param->data items from the queue;
// print them out as they are removed
void *test8th2 (param_t *param)
{
  long sum = 0, item;
  int i; 

  if ( 40 > (int) param->data ) 
    printf ("\nDequeued:"); 
  for ( i = 0; i < (int) param->data; i ++ ) {
    item = (long) param->q->dequeue (-1);
    if ( (int) (param->data) < 40 )
      printf (" %lu", item ); 
    sum += item; 
  } 
  if ( 40 > (int) param->data )
    printf ("\n");

  return ((void*)sum);
}

// test 10
// Consumer, producer, and prosumer

// Producer: enqueue param->data items to the queue;
void *test10th1 (param_t *param)
{
  long sum = 0, item;
  int i;

  for ( i = 0; i < (int) param->data; i ++ ) {
    item = i+1;
    param->q->enqueue ((void*) item);
    sum += item;
  }

  return ((void*)sum);
}

// Consumer: dequeue param->data items from the queue;
void *test10th2 (param_t *param)
{
  long sum = 0, item;
  int i;

  for ( i = 0; i < (int) param->data; i ++ ) {
    item = (long) param->q->dequeue (-1); // block
    sum += item;
  }

  return ((void*)sum);
}

// Prosumer: enqueue a half of param->data items to the queue;
// then dequeue a half of para->data items.
// ASSUME param->data is even.
void *test10th3 (param_t *param)
{
  long balance = 0, item, offset;
  int i;

  offset = (long) param->data * ((long) param->data +1);
  offset /= 2;
  for ( i = 0; i < (int) param->data /2; ++i ) {
    item = offset + i+1;
    param->q->enqueue ((void*) item); 
    balance += item;
    item = (long) param->q->dequeue (-1); // block
    balance -= item;
  }

  return ((void*)balance);
}

// test 11
// Multiple queues

// Prosumer: suck off one queue, fill up the other one
void *test11th1 (param_t *param)
{
  long item;
  long balance = 0L;
  long i;

  for ( i = 0; i < (long) param->data; ++i ) {
    item = i +1;
    param->q->enqueue ((void*)item);
    balance += item;
    item = (long) param->qa->dequeue (-1); // block
    balance -= item;
  }

  return ((void*)balance);
}

// Prosumer: suck off one queue, fill up the other one
void *test11th2 (param_t *param)
{
  long item;
  long balance = 0L;
  long i;

  for ( i = 0; i < (long) param->data; ++i ) {
    item = (long) param->q->dequeue (-1); // block
    balance -= item;
    item = i +1 + 100000L;
    param->qa->enqueue ((void*)item);
    balance += item;
  }

  return ((void*) balance);
}

// .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..

int test [] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

void select_tests (int c, char **v)
{
  int i, t;
  int test_count;

  test_count = sizeof (test) / sizeof (*test);
  for (i = 1; i < c; ++i) {
    t = atoi (v [i]);
    if ( 0 == t && v [i][0] != '0' ) {
      fprintf (stderr, "***List tests NOT to perform\n");
      exit (0);
    }
    test [abs (t)] = 0;
  }
}

main (int argc, char *argv[])
{
  int grade [] = {
    5,  // 0) object storage
    5,  // 1) probe on empty
    5,  // 2) head on empty
    5,  // 3) probe on non-empty
    5,  // 4) head on non-empty
    5,  // 5) timeout
    5,  // 6) underflow
    5,  // 7) overflow
    15, // 8) required test
    5,  // 9) same, larger input
    10, // 10) producer/consumer/prosumer
    10  // 11) two queues, two prosumers
    // 80 total. Add 10 points for maintaining local mutex and
    // conditions per queue. Add 5 points for the correctness and
    // completeness of submission. Add 5 points for clean compilation.
    // Total 100.
  };
  int total = 0;
  int maximum = 0;
  pthread_t th1, th2, th3;
  unsigned long u1 = 57, u2 = 179, u3, u4;
  void *v1, *v2, *v3, *v4, *v5;

  Queue *q = NULL, *qa = NULL;
  param_t param1, param2, param3;

  struct timeval tv_start, tv_end;
  unsigned long time_correction, time_eps;

  int test_id;
  select_tests (argc, argv);

  /*
   * One thread now: singular queue handling.
   */
  out ("\n\t\t[Single threads]\n");

  test_id = 0;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing object storage: ", test_id );
    q = new Queue (15);
    v1 = &u1;
    q->enqueue (v1);
    v2 = q->dequeue(0);
    if ( v1 == v2 ) {
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("ERROR\n");
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 1;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing probe on empty queue: ", test_id );
    q = new Queue (10);
    if ( ! q->probe () && ! q->probe() ) { // make sure it releases mutex
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("ERROR");
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 2;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing head on empty queue: ", test_id );
    q = new Queue (5);
    if ( NULL == q->head () && NULL == q->head () ) { // make sure it
      // releases mutex
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("ERROR");
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 3;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing probe on non-empty queue: ", test_id );
    q = new Queue (10);
    q->enqueue (&u1);
    if ( q->probe () && q->probe () ) { // make sure it releases mutex
      out ("ok\n");
      total += grade [3];
    }else {
      out ("ERROR");
    }
    delete q;
    maximum += grade [3];

  }

  test_id = 4;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing head on non-empty queue: ", test_id );
    q = new Queue (10);
    v2 = &u2;
    q->enqueue (v2);
    if ( v2 == q->head () && v2 == q->head () ) { // make sure it
      // releases mutex and
      // keeps head in queue
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("ERROR");
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 6;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing underflow: ", test_id );
    q = new Queue (2);
    q->enqueue (&u1);
    q->dequeue (0);
    q->dequeue (0);
    if ( NULL == q->head() && ! q->probe() ) {
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("ERROR\n");
    }
    delete q;
    maximum += grade [test_id];

    /*
     * Several threads. Producer/Consumer.
     */
    out ("\n\t\t[Multiple threads]\n");

  }

  test_id = 7;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing overflow: ", test_id );
    q = new Queue (2);
    v1 = &u1; v2 = &u2; v3 = &u3; v4 = &u4; v5 = NULL;
    q->enqueue (v1);
    q->enqueue (v2);
    param2.q = param1.q = q;
    param1.data = v3;
    pthread_mutex_init (&me, NULL);
    pthread_cond_init (&cond, NULL);
    pthread_create (&th1, NULL, test7th1, &param1 );
    pthread_create (&th2, NULL, test7th2, &param2 );
    pthread_join (th1, &v4);
    pthread_join (th2, &v5);
    pthread_cond_destroy (&cond);
    pthread_mutex_destroy (&me);
    if ( v1 != q->head() && v3 != q->head() && v2 != q->head() ) {
      fprintf (stderr,
        "ERROR head=%#0x v1=%#0x v2=%#0x v3=%#0x v4=%#0x v5=%#0x",
        (unsigned) q->head(), (unsigned) v1, (unsigned) v2,
        (unsigned) v3, (unsigned) v4, (unsigned) v5
      );
    }else if ( v2 != v5 && v1 != v5 ) {
      fprintf (stderr,
        "ERROR enqueue v1=%#0x v2=%#0x v3=%#0x v4=%#0x v5=%#0x\n",
        (unsigned) v1, (unsigned) v2, (unsigned) v3,
        (unsigned) v4, (unsigned) v5
      );
    }else {
      out ("ok\n");
      total += grade [test_id];
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 8;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d)Testing required: ", test_id );
    q = new Queue (5);
    pthread_mutex_init (&me, NULL);
    pthread_cond_init (&cond, NULL);
    param2.q = param1.q = q;
    param2.data = param1.data = (void*) 10;
    pthread_create (&th1, NULL, test8th1, &param1 );
    pthread_create (&th2, NULL, test8th2, &param2 );
    pthread_join (th1, &v1);
    pthread_join (th2, &v2);
    fprintf (stderr, "Sum of items: enqueued %d, dequeued %d\n",
      (int) v1, (int) v2 );
    pthread_cond_destroy (&cond);
    pthread_mutex_destroy (&me);
    if ( q->probe () || NULL != q->head () ) {
      out ("ERROR queue\n");
    }else if ( (unsigned long) v1 != (unsigned long) v2 ) {
      out ("ERROR balance\n");
    }else if ( 2 * (unsigned long) v1 !=
      (unsigned long) param1.data * ( (unsigned long) param1.data +1)
    ) {
      fprintf (stderr, "ERROR sum=%lu, expected=%lu\n",
        (unsigned long) v1,
        (unsigned long) param1.data * ( (unsigned long) param1.data +1) /2
      );
    } else {
      out ("ok\n");
      total += grade [test_id];
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 9;
  if ( test [test_id] ) {
    fprintf (stderr,
      "\n--\n%d) Testing required, only larger input.\n", test_id );
    q = new Queue (5);
    pthread_mutex_init (&me, NULL);
    pthread_cond_init (&cond, NULL);
    param2.q = param1.q = q;
    param2.data = param1.data = (void*) 10000;
    pthread_create (&th1, NULL, test8th1, &param1 );
    pthread_create (&th2, NULL, test8th2, &param2 );
    pthread_join (th1, &v1);
    pthread_join (th2, &v2);
    fprintf (stderr, "Sum of items: enqueued %lu, dequeued %lu\n",
      (unsigned long) v1, (unsigned long) v2 );
    pthread_cond_destroy (&cond);
    pthread_mutex_destroy (&me);
    if ( q->probe () || NULL != q->head () ) {
      out ("ERROR queue\n");
    }else if ( (unsigned long) v1 != (unsigned long) v2 ) {
      out ("ERROR balance\n");
    }else if ( 2 * (unsigned long) v1 !=
      (unsigned long) param1.data * ( (unsigned long) param1.data +1)
    ) {
      fprintf (stderr, "ERROR sum=%lu, expected=%lu\n",
        (unsigned long) v1,
        (unsigned long) param1.data * ( (unsigned long) param1.data +1) /2
      );
    } else {
      out ("ok\n");
      total += grade [test_id];
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 10;
  if ( test [test_id] ) {
    fprintf (stderr,
      "\n--\n%d) Testing producer, consumer, and prosumer: ", test_id);
    q = new Queue (10);
    pthread_mutex_init (&me, NULL);
    pthread_cond_init (&cond, NULL);
    param3.q = param2.q = param1.q = q;
    param3.data = param2.data = param1.data = (void*) 5000; // even
    pthread_create (&th1, NULL, test10th1, &param1 );
    pthread_create (&th2, NULL, test10th2, &param2 );
    pthread_create (&th3, NULL, test10th3, &param3 );
    pthread_join (th1, &v1);
    pthread_join (th2, &v2);
    pthread_join (th3, &v3);
    fprintf (stderr,
      "\nSum of items: producer %ld, consumer %ld, prosumer balance %ld\n",
      (long) v1, (long) v2,
      (long) v3
    );
    pthread_cond_destroy (&cond);
    pthread_mutex_destroy (&me);
    if ( q->probe () || NULL != q->head () ) {
      out ("ERROR queue\n");
    }else if ( (long) v1 + (long) v3 != (long) v2 ) {
      out ("ERROR balance\n");
    }else {
      out ("ok\n");
      total += grade [test_id];
    }
    delete q;
    maximum += grade [test_id];

  }

  test_id = 11;
  if ( test [test_id] ) {
    fprintf (stderr,
      "\n--\n%d) Testing two queues, two prosumers: ", test_id );
    q = new Queue (10);
    qa = new Queue (20);
    pthread_mutex_init (&me, NULL);
    pthread_cond_init (&cond, NULL);
    param2.q = param1.q = q; 
    param2.qa = param1.qa = qa;
    param1.data = param2.data = (void*) 2000; // any number
    pthread_create (&th1, NULL, test11th1, &param1 );
    pthread_create (&th2, NULL, test11th2, &param2 );
    pthread_join (th1, &v1);
    pthread_join (th2, &v2);
    fprintf (stderr,
      "\nBalance of items: one %ld, another %ld\n",
      (long) v1, (long) v2
    );
    pthread_cond_destroy (&cond);
    pthread_mutex_destroy (&me);
    if ( q->probe () || qa->probe () ||
      NULL != q->head () || NULL != qa->head ()
    ) {
      out ("ERROR queue\n");
    }else if ( (long) v1 + (long) v2 != 0 ) {
      out ("ERROR balance\n");
    }else {
      out ("ok\n");
      total += grade [test_id];
    }
    delete q; delete qa;
    maximum += grade [test_id];

  }

  test_id = 5;
  if ( test [test_id] ) {
    fprintf (stderr, "\n--\n%d) Testing timeout: ", test_id );
    q = new Queue (2);
    if ( q->probe () ) q->dequeue (1);
    if ( q->probe () ) q->dequeue (1);
    if ( q->probe () ) q->dequeue (1);
    u2 = 2000000; // 2 sec
    gettimeofday (&tv_start, NULL);
    gettimeofday (&tv_end, NULL);
    time_correction = time_elapsed (tv_start, tv_end);
    gettimeofday (&tv_start, NULL);
    q->dequeue (u2);
    gettimeofday (&tv_end, NULL);
    u1 = time_elapsed (tv_start, tv_end) - time_correction;
    fprintf (stderr,
      "time elapsed %lu usec, expected %lu usec\n\t(correction %lu); ",
      u1, u2, time_correction
    );
    time_eps = max2 (time_correction, 500000); // up to 500 msec is okay
    if ( (unsigned long) labs (u1 - u2) < time_eps) { // timeout discrepancy
      out ("ok\n");
      total += grade [test_id];
    }else {
      out ("TIMEOUT ASSUMED INCORRECT\n");
    }
    delete q;
    maximum += grade [test_id];
  }

  fprintf (stderr, "\n\n--\nTotal grade %d out of maximum %d\n",
    total, maximum );
  
} // main()
