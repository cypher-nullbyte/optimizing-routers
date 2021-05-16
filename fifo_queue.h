#ifndef FIFO_QUEUE_H_
#define FIFO_QUEUE_H_

struct node {
    struct node* next;
    int *router_socket;
};
int* dequeue();
void enqueue(int* router_socket);
#endif