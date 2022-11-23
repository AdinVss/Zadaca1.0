#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *connection_handler();


int main (void){
    int listenfd=0;
    int c;
    int bystesReceived=0;
    int conndf=0;
    struct sockaddr_in serv_addr, client;
    char recvBuff[512];
    char name[100];
    listenfd= socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd==-1)
    {
        printf("Could not create socket");
    }
    printf ("Socket created \n");

    serv_addr.sin_family= AF_INET;
    serv_addr.sin_addr.s_addr= INADDR_ANY;
    serv_addr.sin_port= htons(8888);

    if (bind( listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");
    
    if(listen(listenfd,10)==-1)
    {
        printf("Failed to listen \n");
        return -1;
    }
    puts("Waiting for incoming connection ...");
    c=sizeof(struct sockaddr_in);

    while (1)
    {
        //conndf= accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
    
        connection_handler(listenfd);
        pthread_t t;
        int *pointerTh= malloc(sizeof(int));
        *pointerTh= listenfd;
        pthread_create( &t, NULL, connection_handler, pointerTh);


        if(conndf<0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");
        
        int i=0;
        do
        {
            recv(conndf,recvBuff+i,1,0);
        } while(recvBuff[i++]!='\n');

        recvBuff[i-1]='\0';
        FILE *fp;
        fp = fopen (recvBuff, "wb");

        if( fp==NULL)
        {
            printf("Error opening file");
            return 1;
        }
        while ((bystesReceived=recv(conndf,recvBuff,
        sizeof(recvBuff),0))>0)
        {
            fwrite(recvBuff,1,bystesReceived,fp);
        }
        if (bystesReceived==0)
        {
            puts("Client disconnected");
        }
        else if (bystesReceived==-1)
        {
            perror("rec failed.");
        }
        close(conndf);
        fclose(fp);
    }
    return 0;
}

void *connection_handler(int *pointerSocket)
{


}