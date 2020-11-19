#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

#define PORT 8000
long long int filesize;
long long int total=0;

void writefile(int fd, int socket)
{
    long long int n;
    char buffer[1024] = "";
    printf("Starting reading..\n");
    total=0;
    while ((n = recv(socket, buffer, 1024, 0)) > 0) 
    {
        total = total + n;

        printf("Received %lld %%\n", (long long int)(100*total/filesize));
        if (n == -1)
        {
            perror("Error in receiving file");
            exit(1);
        }

        if (write(fd, buffer, n) == -1)
        {
            perror("Error in writing file");
            exit(1);
        }

        memset(buffer, 0, 1024);

        if(total >= filesize)
        {
            break;
        }
    }
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    
    char filename[1024] = "";

    while(1)
    {
        total=0;
        printf("client -> ");

        //Input command
        char *command = NULL;
        size_t len;
        getline(&command, &len, stdin);

        // Get subcommands out
        char *withincommands[1000];
        withincommands[0] = strtok(command, "\n ");
        int numwithincom = 0;
        while (withincommands[numwithincom] != NULL) 
        {  
            numwithincom++;
            withincommands[numwithincom] = strtok(NULL, "\n "); 
        } 

        if(strcmp(withincommands[0], "get")==0)
        {

            for(long long k = 1; k < numwithincom; k++)
            {
                strcpy(filename, withincommands[k]);
                send(sock, filename, strlen(filename), 0);

                char size[1024] = "";
                if (recv(sock, size, 1024, 0) == -1) 
                {
                    perror("Error in receiving file size");
                    exit(1);
                }
                filesize = atoi(size);

                if(filesize == -1)
                {
                    printf("This file does not exist in the server\n");
                    continue;
                }
                
                int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);

                if (file == -1) 
                {
                    perror("Could not create the file");
                    exit(1);
                }

                writefile(file, sock);

            }
        }

        else if(strcmp(withincommands[0], "exit")==0)
        {

            send(sock, "exit", strlen("exit"), 0);
            printf("Closing the connection to server\n");
            close(sock);
            break;

        }
        else
        {
            printf("Invalid command: Only get and exit are allowed\n");
        }

    }



}
