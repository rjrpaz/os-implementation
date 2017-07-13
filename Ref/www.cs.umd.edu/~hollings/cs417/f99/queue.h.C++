
/*
 * Header file for the Queue abstraction - C++ version.
 *
 */

class Queue {
  public:
    // init the queue abstraction to hold at most macItems
    Queue(int maxItems);

    // enqueue - add item to the end of queue, block if the queue is full
    void enqueue(void *item);

    // dequeue - remove an item from queue.  Wait up to timeout usec for an item
    void *dequeue(int timeout);

    // head - return the first item in the queue, but don't dequeue it
    void *head();

    // probe - return true if the queue contains at least one item
    bool probe();
  private:
    // you can add code here if you want to.
};
