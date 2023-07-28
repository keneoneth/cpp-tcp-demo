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

## Run client
open one or more terminals, execute `make run_client` on each of them to test the server for simultaneuously accepting connections from >1 clients  
