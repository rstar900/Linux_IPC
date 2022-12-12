#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h> // for sockaddr_un (Unix domain socket address data structure)
#include <signal.h>

#define SOCK_ADDR "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int conn_socket_fd = -1; // connection socket file descriptor

void sigHandler(int signo)
{
    close(conn_socket_fd);
    printf("Closed connection socket\n");
    exit(EXIT_SUCCESS);
}

int main()
{
    // Flow for a server
    // socket() -> bind() -> listen() -> accept() -> read()/write() -> close()

    // signal handler for closing connections
    signal(SIGINT, sigHandler);

    unlink(SOCK_ADDR); // Close the previous

    // Structure for holding address
    struct sockaddr_un name; 
    memset(&name, 0, sizeof(name)); // clearing the structure
    name.sun_family = AF_UNIX; // Unix Domain Socket (Local communication)
    strcpy(name.sun_path, SOCK_ADDR); // asigining the address

    int result, data_socket_fd, ret, value, clientExited = 0;
    char buffer[BUFFER_SIZE];
    
    // make a connection socket (master socket file descriptor)
    conn_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (conn_socket_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Created connection socket\n");

    // bind the socket with the address
    ret = bind(conn_socket_fd, (const struct sockaddr*)(&name), sizeof(name));

    if(ret == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("Bind successful\n");

    // Tell the OS that this socket is for accepting incoming connections using accept() call with queue of 20 connections
    ret = listen(conn_socket_fd, 20);

    if(ret == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Listen successful\n");

    // Now accept incoming connections, create for each a data socket and do read/write operations
    while (1)
    {
        printf("Accepting connections\n");
        data_socket_fd = accept(conn_socket_fd, NULL, NULL);

        if (data_socket_fd == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Accepted connection\n");

        result = 0; // initialize the result for operations
        memset(buffer, 0, BUFFER_SIZE); // empty the buffer
        do
        {
            ret = read(data_socket_fd, buffer, sizeof(int)); // reading int

            if (ret == -1)
            {
                perror("read");
                exit(EXIT_FAILURE);
            }

            else if (ret == 0)
            {
                printf("Client exited\n");
                clientExited = 1;
                break;
            }

            value = *((int*)buffer); // read buffer's first 4 bytes as an integer
            printf("Read %i\n", value);
            result += value; // add them to result

        } while (value); // while value is not 0

        // skip writing in case client exited
        if (clientExited)
        {
            clientExited = 0;
            continue;
        }

        // send back result as a string and close connection
        sprintf(buffer, "The result is %i", result);
        ret = write(data_socket_fd, buffer, BUFFER_SIZE);

        if(ret == -1)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }

        printf("Wrote the result : (%s)\n", buffer);
        close(data_socket_fd); // close the data socket
    }

    return 0;

}