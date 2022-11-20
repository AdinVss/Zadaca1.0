#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, c, bytesReceived;
    struct sockaddr_in server, client;
    char recvBuffer[512];
    char name[100];

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
    if (listen(socket_desc, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    while (1)
    {
        // Accept and incoming connection
        puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);

        // accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
        if (client_sock < 0)
        {
            perror("Accept failed");
            return 1;
        }
        puts("Connection accepted");

        if (fork() == 0)
        {
            close(socket_desc);

            // find name of the incoming file
            // recive 1 byte by iteration until we find new line

            int i = 0;
            do
            {
                recv(client_sock, recvBuffer + i, 1, 0);
            } while (recvBuffer[i++] != '\n');

            // remove \n and terminate string
            recvBuffer[i - 1] = '\0';

            FILE *fp;
            fp = fopen(recvBuffer, "wb");

            if (NULL == fp)
            {
                printf("Error opening file");
                return 1;
            }
            printf("Successfully created a file.\n");

            // Receive data in chunks of 512 bytes

            while ((bytesReceived = recv(client_sock, recvBuffer, sizeof(recvBuffer), 0)) > 0)
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
            close(client_sock);
            return 0;

        }
        close(client_sock);
        
    }

    return 0;

}