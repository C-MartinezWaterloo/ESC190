#if !defined(QUEUE)
#define QUEUE

typedef struct Queue {
    int* data;
    int size;
    int size_inclusive;
    int capacity;
    int initial_capacity;
    int front;
    int rear;
} Queue;

void print(Queue* q);

void enqueue(Queue* q, int data);

int dequeue(Queue* q);

void destroy_queue(Queue* q);

void init(Queue** q);

#endif
