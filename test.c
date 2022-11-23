#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

int main (void){
    int listenfd=0;
    int c;
    int bystesReceived=0;
    int conndf=0;
    struct sockaddr_in serv_addr, client;
    char recvBuff[512];
    char name[100];
    int k=0;
    char z;
    listenfd= socket(AF_INET, SOCK_STREAM,0);

    if (listenfd==-1)
    {
        printf("Could not create socket");
    }
    printf ("Socket created \n");

    serv_addr.sin_family= AF_INET;
    serv_addr.sin_addr.s_addr= INADDR_ANY;
    serv_addr.sin_port= htons(8888);

    if (bind(listenfd,(struct sockaddr *)&serv_addr, 
    sizeof(serv_addr))<0)
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
        conndf= accept(listenfd, (struct sockaddr *)&client, (socklen_t *)&c);
        if(conndf<0)
        {
            perror("Accept failed");
            return 1;
        }
        puts("Connection accepted");

        //pid_t forkStatus = fork();
            //    printf("%d",forkStatus);
        printf("%d",fork());
        if (fork()==0)
        {
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
            while ((bystesReceived=recv( conndf, recvBuff, sizeof(recvBuff),0))>0)
            {
                fwrite(recvBuff,1,bystesReceived,fp);
            }
            fclose(fp);
            k=5;
            _Exit(3);
        }
        else if (fork()>0)
        {
            if (bystesReceived==0)
            {
                puts("Client disconnected");
            }
            else if (bystesReceived==-1)
            {
                perror("rec failed.");
            }
        }
        else 
        {
            printf("Error while creating procces");
            return 1;
        }
        close(conndf);
    }
    return 0;
}