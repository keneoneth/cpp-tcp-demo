#include <iostream>
#include <cstring>

extern "C"
{
#include <arpa/inet.h>
#include <unistd.h>
}

#ifndef MSSG_SEND_PERIOD_SEC
#define MSSG_SEND_PERIOD_SEC 1
#endif

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

// defines how the sent message looks like
#define MSSG_TEMPLATE "'sent message count: %u'"

using namespace std;

int create_client_socket()
{

    const int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) // failed condition
    {
        throw std::runtime_error("socket creation failed with error " + std::string(strerror(errno)));
    }
    else
    {
        return socket_fd;
    }
}

sockaddr_in connect_socket_in_addr(const int &socket_fd, char *&server_ip, const int &server_port)
{
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);

    const int inet_pton_ret = inet_pton(
        AF_INET,      // int af
        server_ip,    // const char *restrict src
        &sin.sin_addr // void *restrict dst
    );

    if (inet_pton_ret == -1) // failed condition
    {
        throw std::runtime_error("socket connection to server failed with error " + std::string(strerror(errno)));
    }

    const int connect_ret = connect(
        socket_fd,               // int socket
        (struct sockaddr *)&sin, // struct sockaddr *address
        sizeof(sin));            // int address_len

    if (connect_ret == -1) // failed condition
    {
        throw std::runtime_error("socket connection to server failed with error " + std::string(strerror(errno)));
    }
    return sin;
}

void start_client_sending(const int &socket_fd)
{
    char *buff = (char *)malloc(BUFF_SIZE * sizeof(char));
    unsigned int message_count = 0;

    while (true)
    {
        sprintf(buff, MSSG_TEMPLATE, ++message_count);

        const int send_ret = send(
            socket_fd, // int sockfd
            buff,      // const void *buf
            BUFF_SIZE, // size_t len
            0          // int flags
        );

        if (send_ret == -1) // failed condition
        {
            throw std::runtime_error("sending message to server failed with error " + std::string(strerror(errno)));
        }

        printf("Successful SEND message to server: %s\n", buff);

        // clear message in buffer
        memset(buff, 0, BUFF_SIZE);

        const int recv_ret = recv(
            socket_fd, // int socket
            buff,      // char *buffer
            BUFF_SIZE, // int length
            0          // int flags
        );

        if (recv_ret == -1) // failed condition
        {
            throw std::runtime_error("receiving echoed message from server failed with error " + std::string(strerror(errno)));
        }

        printf("Successful RECV message from server: %s\n", buff);

        // // sleep for some time before resending new messages
        sleep(MSSG_SEND_PERIOD_SEC);
    }

    free(buff); // deallocate buffer
}

int main(int argc, char *argv[])
{

    printf("hello from client\n"); // just to say hi

    if (argc != 3) // arguments number is not as expected
    {
        std::cerr << "Error: please provide server ip and server port in the arguments i.e. " << argv[0] << " {server_ip} {server_port}" << std::endl;
        exit(EXIT_FAILURE);
    }

    // retreive the ip addr and port number
    char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    // Step 1: create the socket
    auto socket_fd = create_client_socket();
    printf("created socket file descriptor: %d\n", socket_fd);

    // Step 2:connect socket to server's ip/port
    connect_socket_in_addr(socket_fd, server_ip, server_port);

    // Step 3: start sending messages periodically
    start_client_sending(socket_fd);

    return EXIT_SUCCESS;
}