
/*
 * Header file for the Queue abstraction - C version.
 *
 */

typedef int bool;
typedef struct _Queue Queue;

/* init the queue abstraction to hold at most macItems */
Queue *createQueue(int maxItems);

/* enqueue - add item to the end of queue, block if the queue is full */
void enqueue(Queue*, void *item);

/* dequeue - remove an item from queue.  Wait up to timeout usec for an item */
void *dequeue(Queue*, int timeout);

/* head - return the first item in the queue, but don't dequeue it */
void *head(Queue*);

/* probe - return true if the queue contains at least one item */
bool probe(Queue*);

