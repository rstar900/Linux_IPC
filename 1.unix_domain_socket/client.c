#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h> // for sockaddr_un (Unix domain socket address data structure)

#define SOCK_ADDR "/tmp/DemoSocket"
#define BUFFER_SIZE 128

int main()
{
    // Flow for a client
    // socket() -> connect() -> read()/write() -> close(); 

// Structure for holding address
    struct sockaddr_un name; 
    memset(&name, 0, sizeof(name)); // clearing the structure
    name.sun_family = AF_UNIX; // Unix Domain Socket (Local communication)
    strcpy(name.sun_path, SOCK_ADDR); // asigining the address

    int data_socket_fd, ret, value;
    char buffer[BUFFER_SIZE];
    
    // make a data socket
    data_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (data_socket_fd == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Created data socket\n");

    // Use connect() to connect to the server
    ret = connect(data_socket_fd, (const struct sockaddr*)(&name), sizeof(name));

    if (ret == -1)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected successfully\n");

    // Input numbers (to end: enter 0) and send to server
    do
    {
        printf("Enter a number (0 to end):\n");
        scanf("%i",&value);

        // Send to server 
        ret = write(data_socket_fd, &value, sizeof(int));
        if(ret == -1)
        {
            perror("write");
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            
        }
        printf("Sent %i to the server\n", value);

    } while (value);

    // Read the result string from the server
    ret = read(data_socket_fd, buffer, BUFFER_SIZE); // reading int

    if (ret == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Read the result : (%s)\n", buffer);
    close(data_socket_fd); // close the data socket

    return 0;
    
}