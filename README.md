# cpp-tcp-demo

Demo of server client connection over TCP protocol  
Requirements:  
1. a TCP server listening on a port (e.g.,8080); it could accept multiple clients, and echo back clients' messages 
2. a TCP client, which could: 
a. read command line options for server IP/port 
b. periodically (e.g., every second) send a message (e.g., a sent message count) to the server, and print out server's response.  

## Setup
- Install libevent by `apt-get install libevent-dev`
- GCC compiler

## Build server & client
execute `make build`  
Note: To build server or client individually, execute `make build_client` or execute `make build_client`  

## Run server
open a terminal, execute `make run_server`  
Expected print in the console: 
```
./bin/server_exe
hello from server
created socket file descriptor: 3
created socket option: 32681
start server listening at 0.0.0.0/8080 ...
```

## Run client
open one or more terminals, execute `make run_client SERVER_IP=127.0.0.1 SERVER_PORT=8080` on each of them to test the server for simultaneously accepting connections from >1 clients 
Once connected, the client will send a message count info. to the server periodically (e.g. every 1 sec)  
Note: you may kill the process anytime by ctrl-c to stop the client  

Expected print in the client's console: 
```
./bin/client_exe 127.0.0.1 8080
hello from client
created socket file descriptor: 3
Successfully SEND message to server: 'sent message count: 1'
Successful RECV message from server: 'sent message count: 1'
Successfully SEND message to server: 'sent message count: 2'
Successful RECV message from server: 'sent message count: 2'
Successfully SEND message to server: 'sent message count: 3'
Successful RECV message from server: 'sent message count: 3'
```

And expected print in the server's console:  
```
accepted connection socket 7
read data from socket 7: 'sent message count: 1'
sent data to socket 7: 'sent message count: 1'
read data from socket 7: 'sent message count: 2'
sent data to socket 7: 'sent message count: 2'
read data from socket 7: 'sent message count: 3'
sent data to socket 7: 'sent message count: 3'
```

## Cleanup
execute `make clean`  

## Change configs  
The Makefile defines the following parameters for easy modifications
CC=g++ # specifies the compiler to use  
PORT=8080 # specifies the port number the server listens to  
BUFF_SIZE=8192 # specifies the buffer size of the TCP socket  
MSSG_SEND_PERIOD_SEC=1 # specifies how frequent the message is sent from client to server (in terms of seconds)  
