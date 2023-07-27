# this Makefile is written for build and run of the TCP server client demo


CC=g++
PORT=8080
CFLAGS=-std=c++11 -DPORT=$(PORT)

build_server:
	$(CC) ./server_end/server.cpp $(CFLAGS) -o ./bin/server_exe

build_client:
	$(CC) ./client_end/client.cpp $(CFLAGS) -o ./bin/client_exe

build: build_client build_server

run_server:
	./bin/server_exe

run_client:
	./bin/client_exe