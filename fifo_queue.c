#include "fifo_queue.h"
#include<stdlib.h>

typedef struct node node_t;

node_t* head=NULL;
node_t* tail=NULL;

void enqueue(int *router_socket)
{
    node_t* newnode=malloc(sizeof(node_t));
    newnode->router_socket=router_socket;
    newnode->next=NULL;
    if(tail==NULL)
    {
        head=newnode;
    }
    else
    {
        tail->next=newnode;
    }
    tail=newnode;
}


int* dequeue()
{
    if(head==NULL) return NULL;
    else
    {
        int *result=head->router_socket;
        node_t* temp=head;
        head= head->next;
        if(head==NULL) {tail=NULL;}
        free(temp);
        return result;
    }
}