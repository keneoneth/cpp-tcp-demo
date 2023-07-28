# this Makefile is written for build and run of the TCP server client demo


CC=g++ # specifies the compiler to use
PORT=8080 # specifies the port number the server listens to
BUFF_SIZE=8192 # specifies the buffer size of the TCP socket
MSSG_SEND_PERIOD_SEC=1 # specifies how frequent the message is sent from client to server (in terms of seconds)

CFLAGS=-std=c++11 -DPORT=$(PORT) -DBUFF_SIZE=$(BUFF_SIZE)

build_server:
	$(CC) ./server_end/server.cpp $(CFLAGS) -levent -o ./bin/server_exe

build_client:
	$(CC) ./client_end/client.cpp $(CFLAGS) -o ./bin/client_exe

build: build_client build_server

run_server:
	./bin/server_exe

run_client:
	./bin/client_exe $(SERVER_IP) $(SERVER_PORT)

clean:
	rm ./bin/server_exe
	rm ./bin/client_exe