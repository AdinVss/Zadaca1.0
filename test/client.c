#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{   
    int sockfd=0;
    char sendBuffer[512];
    int bytesRead=0;
    struct sockaddr_in serv_addr;

    if(argc !=2){
        printf(" Wrong number of arguments \n");
        return 1;
    }
    if ((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
    {
        printf("\n Error: Could not create socket \n");
        return 1;
    }
    puts("Socket created");

    serv_addr.sin_addr.s_addr=INADDR_ANY;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(8888);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,
    sizeof(serv_addr))<0)
    {
        printf("\n Error: Connection Failed \n");
        return 1;
    }
    puts("Connected \n");

    FILE *fp;
    puts(argv[1]);
    fp=fopen(argv[1],"rb");
    if (fp==NULL)
    {
        printf("File open error");
        return 1;
    }

    int size = strlen(argv[1]);
    char *name= malloc(size+2);
    strcpy (name , argv[1]);
    name[size]='\n';
    name [size + 1]= '\0';
    send (sockfd, name, strlen(name),0);
    free(name);
    
    while(!feof(fp))
    {
        bytesRead=fread(sendBuffer,1,sizeof(sendBuffer),fp);
        if (send(sockfd,sendBuffer, bytesRead, 0)<0)
        {
            puts("Send fialed");
            return 1;
        }
    }
    fclose(fp);
    close(sockfd);
    return 0;
}
