#include <iostream>
#include <cstring>

extern "C"
{
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <event.h>
}

#ifndef PORT
#define PORT 0
#endif

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

using namespace std;

struct EventArgs
{
    sockaddr_in sin;
};

struct Client
{
    event ev_read;
};

int create_server_socket()
{

    const int socket_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (socket_fd == -1) // failed condition
    {
        throw std::runtime_error("socket creation failed with error " + std::string(strerror(errno)));
    }
    else
    {
        return socket_fd;
    }
}

void set_socket_opt(const int &socket_fd, int &opt)
{
    const int setsockopt_ret = setsockopt(
        socket_fd,                   // int socket
        SOL_SOCKET,                  // int level
        SO_REUSEPORT | SO_REUSEADDR, // int option_name (SO_REUSEPORT : allows binding of an arbitrary number of sockets to the samep IP/PORT | SO_REUSEADDR : allows binding of socket unless there's a conflict with the exact IP/PORT (ignoring wildcard IP/PORT))
        &opt,                        // const void *option_value
        sizeof(opt)                  // option_len
    );
    if (setsockopt_ret == -1) // failed condition
    {
        throw std::runtime_error("socket option set failed with error " + std::string(strerror(errno)));
    }
}

// TODO: can remove?
// void set_socket_nonblocking(const int &socket_fd)
// {
//     // set socket to non-blocking mode
//     int flags;

//     flags = fcntl(socket_fd, F_GETFL); // get flags

//     if (flags == -1) // failed condition
//     {
//         throw std::runtime_error("socket get flag failed with error " + std::string(strerror(errno)));
//     }

//     flags |= O_NONBLOCK; // add non-blocking flag

//     const int ret_fcnt1 = fcntl(socket_fd, F_SETFL, flags);

//     if (ret_fcnt1 == -1) // failed condition
//     {
//         throw std::runtime_error("socket set flag failed with error " + std::string(strerror(errno)));
//     }
// }

sockaddr_in bind_socket_in_addr(const int &socket_fd)
{
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    const int bind_ret = bind(
        socket_fd,               // int socket
        (struct sockaddr *)&sin, // const struct sockaddr *address
        sizeof(sin)              // socklen_t address_len
    );

    if (bind_ret == -1) // failed condition
    {
        throw std::runtime_error("socket bind with in addr failed with error " + std::string(strerror(errno)));
    }
    return sin;
}

void on_read(int client_fd, short ev, void *arg)
{
    Client * client = (Client *)arg;

    char *buff = (char *)malloc(BUFF_SIZE * sizeof(char));

    const int read_ret = read(
        client_fd, // int fs
        buff,      // void *buf
        BUFF_SIZE  // size_t len
    );

    if (read_ret <= 0) // cannot read any data
    {
        const int close_ret = close(client_fd); // close connected socket

        if (close_ret == -1) // failed condition
        {
            throw std::runtime_error("socket connection closure failed with error " + std::string(strerror(errno)));
        }
        printf("closed connection with socket %d after fail read\n", client_fd);

        event_del(&(client->ev_read)); // delete event
        free(client);                  // deallocate client
        free(buff);                    // deallocate buffer
        return;
    }

    // log read data in console
    printf("read data from socket %d: %s\n", client_fd, buff);

    // echo received message
    const int send_ret = send(
        client_fd, // int sockfd
        buff,      // const void *buf
        BUFF_SIZE, // size_t len
        0          // int flags
    );

    printf("sent data to socket %d: %s\n", client_fd, buff);

    if (send_ret <= 0) // cannot send any data
    {
        const int close_ret = close(client_fd); // close connected socket

        if (close_ret == -1) // failed condition
        {
            throw std::runtime_error("socket connection closure failed with error " + std::string(strerror(errno)));
        }
        printf("closed connection with socket %d after fail send\n", client_fd);

        event_del(&(client->ev_read)); // delete event
        free(client);                  // deallocate client
        free(buff);                    // deallocate buffer
        return;
    }

    free(buff); // deallocate buffer
}

void on_accept(int server_fd, short ev, void *arg)
{
    EventArgs *event_args = (EventArgs *)arg; // cast argument back to event args
    const auto sin = event_args->sin;         // retrieve sin

    socklen_t in_addr_len = sizeof(sin);
    int client_fd = accept(
        server_fd,                // int socket
        (struct sockaddr *)&sin,  // struct sockaddr *__restrict__ address
        (socklen_t *)&in_addr_len // socklen_t *__restrict__ address_len
    );

    if (client_fd == -1) // failed condition
    {
        throw std::runtime_error("socket acception of new connection failed with error " + std::string(strerror(errno)));
    }

    printf("accepted connection socket %d\n", client_fd);

    // // declares the socket read event
    Client *client = (Client *)malloc(sizeof(Client));

    event_set(
        &(client->ev_read),   // struct event *ev
        client_fd,            // int fd
        EV_READ | EV_PERSIST, // short event
        on_read,              // void (*fn)(int, short, void *)
        client                // void *arg
    );

    event_add(&(client->ev_read), nullptr); // activate read event
}

void start_server_listening(const int &server_fd, const sockaddr_in &sin)
{

    // listen to connection
    const int listen_ret = listen(server_fd, SOMAXCONN); // allows max number of incoming conn to queue up
    if (listen_ret == -1)                                // failed condition
    {
        throw std::runtime_error("socket listening to new connection failed with error " + std::string(strerror(errno)));
    }

    printf("start server listening at %s/%d ...\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));

    // declare the server socket accept event
    event ev_accept;

    // initialize libevent
    event_init();

    EventArgs event_args;
    event_args.sin = sin;

    event_set(
        &ev_accept,           // struct event *ev
        server_fd,            // int fd
        EV_READ | EV_PERSIST, // short event
        on_accept,            // void (*fn)(int, short, void *)
        (&event_args)         // void *arg
    );
    event_add(&ev_accept, nullptr); // activate accept event

    // starts event loop
    event_dispatch();
}

int main(int argc, char *argv[])
{

    printf("hello from server\n"); // just to say hi

    // Step 1: create the socket
    auto socket_fd = create_server_socket();
    printf("created socket file descriptor: %d\n", socket_fd);

    // Step 2: set up socket options
    int opt;
    set_socket_opt(socket_fd, opt);
    printf("created socket option: %d\n", opt);

    // Step 3: create socket address
    auto sin = bind_socket_in_addr(socket_fd);

    // Step 4: start listening to clients
    start_server_listening(socket_fd, sin);

    return EXIT_SUCCESS;
}