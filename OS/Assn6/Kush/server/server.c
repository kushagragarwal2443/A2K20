#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8000
int total;

void sendfile(int fd, int socket) 
{
    int n; 
    char sendline[1024] = ""; 
    while ((n = read(fd, sendline, 1024)) > 0) 
    {
	    total = total + n;
        
        if(n == -1)
        {
            perror("Error in reading file");
            exit(1);
        }

        printf("Read successfully ...\n");
        
        if(send(socket, sendline, n, 0) == -1)
        {
            perror("Error in sending file");
            exit(1);
        }
        memset(sendline, 0, 1024);

        printf("Sent successfully...\n");
    }

}

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                    (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(0);
    }

    while(1)
    {
        total=0;
        char buffer[1024] = {0};
        char size[1024] = "";

        valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer

        if(strcmp(buffer, "exit")==0){
            printf("Exit was entered on the client side. Server closing...\n");
            close(server_fd);
            exit(0);
        }

        else
        {
            struct stat file_stat;
            int file = open(buffer, O_RDONLY);
            if(file == -1)
            {
                perror("File does not exist");
                int x = send(new_socket, "-1", 1024, 0);
                if(x == -1)
                {
                    perror("Error while sending file size");
                    exit(1);
                }
                continue;
            }
            if (fstat(file, &file_stat) < 0)
            {
                perror("fstat");
                exit(0);
            }

            sprintf(size, "%ld", file_stat.st_size);
            send(new_socket, size, 1024, 0);
            sendfile(file, new_socket);

        }
    }
}
