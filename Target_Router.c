#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h> // close()
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdbool.h>
#include<limits.h>
#include<strings.h>
#include<linux/limits.h>
#include<pthread.h>
#include "fifo_queue.h"

#define PORT 9999
#define BUFSIZE 4096
#define SOCKETERROR (-1)
#define SERVER_BACKLOG_QUEUE_SIZE 100
#define THREAD_POOL_SIZE 10


pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_var=PTHREAD_COND_INITIALIZER;

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;


void handle_connection_fcfs(int client_socket);
void* handle_connection(void* p_client_socket);
int check(int exp, const char *msg);
void * thread_function(void *arg);

int main(int argc,const char ** argv)
{
    int server_socket, client_socket, addr_size;
    SA_IN server_addr, client_addr;

    for(int i=0;i<THREAD_POOL_SIZE;i++)
    {
        pthread_create(&thread_pool[i],NULL,thread_function,NULL);
    }

    check((server_socket=socket(AF_INET, SOCK_STREAM,0)),"Failed to create Socket!");

    //initialize the address struct
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
    server_addr.sin_port=htons(PORT);

    check(bind(server_socket,(SA*)&server_addr,sizeof(server_addr)),"Bind Failed!");

    check(listen(server_socket,SERVER_BACKLOG_QUEUE_SIZE),"Listen Failed!");

    while(true)
    {
        printf("Waiting for Connections...\n");
        addr_size=sizeof(SA_IN);
        check(client_socket=accept(server_socket,(SA*)&client_addr,(socklen_t*)&addr_size),"Accept Failed");
        printf("Connected!\n");

        //--------------M1------------------
        // handle_connection_fcfs(client_socket);
        
        //--------------M2------------------
        // pthread_t t;
        // int *pcilent= malloc(sizeof(int));
        // *pcilent=client_socket;
        // pthread_create(&t,NULL,handle_connection,pcilent);

        //--------------M3------------------
        int *pclient=malloc(sizeof(int));
        *pclient=client_socket;
        pthread_mutex_lock(&mutex);
        enqueue(pclient); //not thread_safe
        pthread_cond_signal(&condition_var);
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

int check(int exp,const char* msg)
{
    if(exp==SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}   

void* thread_function(void* arg)
{
    while(true)
    {
        int* pclient;
        pthread_mutex_lock(&mutex);
        if((pclient=dequeue())==NULL)
        {
            pthread_cond_wait(&condition_var,&mutex);
            pclient=dequeue(); //not thread_safe
        }
        pthread_mutex_unlock(&mutex);
        if(pclient!=NULL)
        {
            handle_connection(pclient);
        }
        else sleep(1);
    }
}

//--------------M1------------------
// void handle_connection_fcfs(int client_socket)
// {
//     char buffer[BUFSIZE];
//     size_t bytes_read;
//     int msgsize=0;
//     char actualpath[PATH_MAX+1];

//     while((bytes_read=read(client_socket,buffer+msgsize,sizeof(buffer)-msgsize-1))>0)
//     {
//         msgsize+=bytes_read;
//         if(msgsize>BUFSIZE-1 || buffer[msgsize-1]=='\n') break;
//     }

//     check(bytes_read,"Recv Error");
//     buffer[msgsize-1]='\0';

//     printf("Request: %s\n",buffer);
//     fflush(stdout);

//     if(realpath(buffer,actualpath)==NULL)
//     {
//         printf("ERROR(bad path): %s\n",buffer);
//         close(client_socket);
//         return;
//     }
//     FILE *fp=fopen(actualpath, "r");
//     if(fp==NULL)
//     {
//         printf("Error(open): %s\n",buffer);
//         close(client_socket);
//         return;
//     }
//     bzero(buffer,sizeof(buffer));
//     while((bytes_read=fread(buffer,1,BUFSIZE,fp))>0)
//     {
//         printf("Sending %zu bytes\n",bytes_read);
//         write(client_socket,buffer,bytes_read);
//     }
//     close(client_socket);
//     fclose(fp);
//     printf("Closing Connection\n");

// }



void* handle_connection(void* p_client_socket)
{
    int client_socket=*((int*)p_client_socket);
    free(p_client_socket);

    char buffer[BUFSIZE];
    size_t bytes_read;
    int msgsize=0;
    char actualpath[PATH_MAX+1];

    while((bytes_read=read(client_socket,buffer+msgsize,sizeof(buffer)-msgsize-1))>0)
    {
        msgsize+=bytes_read;
        if(msgsize>BUFSIZE-1 || buffer[msgsize-1]=='\n') break;
    }

    check(bytes_read,"Recv Error");
    buffer[msgsize-1]='\0';

    printf("Request: %s\n",buffer);
    fflush(stdout);

    if(realpath(buffer,actualpath)==NULL)
    {
        printf("ERROR(bad path): %s\n",buffer);
        close(client_socket);
        return NULL;
    }
    FILE *fp=fopen(actualpath, "r");
    if(fp==NULL)
    {
        printf("Error(open): %s\n",buffer);
        close(client_socket);
        return NULL;
    }
    bzero(buffer,sizeof(buffer));
    while((bytes_read=fread(buffer,1,BUFSIZE,fp))>0)
    {
        printf("Sending %zu bytes\n",bytes_read);
        write(client_socket,buffer,bytes_read);
    }
    close(client_socket);
    fclose(fp);
    printf("Closing Connection\n");
}