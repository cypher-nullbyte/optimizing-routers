#ifndef FIFO_QUEUE_H_
#define FIFO_QUEUE_H_

struct node {
    struct node* next;
    int *client_socket;
};
int* dequeue();
void enqueue(int* client_socket);
#endif