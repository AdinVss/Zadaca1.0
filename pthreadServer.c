#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

// the thread function
void *connection_handler(void *);

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // print the error message
        perror("Bind failed. Error");
        return 1;
    }
    puts("Bind done");

    // Listen
    listen(socket_desc, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    pthread_t thread_id;

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted");

        if (pthread_create(&thread_id, NULL, connection_handler, (void *)&client_sock) < 0)
        {
            perror("Could not create thread");
            return 1;
        }

        // Now join the thread , so that we dont terminate before the thread
        // pthread_join( thread_id , NULL);
        puts("Handler assigned");
    }

    if (client_sock < 0)
    {
        perror("Accept failed");
        return 1;
    }
    
    return 0;
}

// This will handle connection for each client

void *connection_handler(void *socket_desc)
{
    // Get the socket descriptor
    int sock = *(int *)socket_desc;
    int bytesReceived;
    char recvBuffer[512];
    char name[100];

    // find name of the incoming file
    // recive 1 byte by iteration until we find new line

    int i = 0;
    do
    {
        recv(sock, recvBuffer + i, 1, 0);
    } while (recvBuffer[i++] != '\n');

    // remove \n and terminate string
    recvBuffer[i - 1] = '\0';

    FILE *fp;
    fp = fopen(recvBuffer, "wb");

    if (NULL == fp)
    {
        printf("Error opening file");
        exit(1);
    }
    printf("Successfully created a file.\n");

    // Receive data in chunks of 512 bytes

    while ((bytesReceived = recv(sock, recvBuffer, sizeof(recvBuffer), 0)) > 0)
    {
        fwrite(recvBuffer, 1, bytesReceived, fp);
    }
    printf("File update was made.\n");

    if (bytesReceived == 0)
    {
        puts("Client disconnected");
    }
    else if (bytesReceived == -1)
    {
        perror("recv failed");
    }

    fclose(fp);
    close(sock);

    return 0;
}