all: build-server run-server

objects2 = um-bridge-server.cpp httplib.h json.hpp umbridge.h
build-server:
	- g++ -O3 -w -std=c++11 $(objects2) -o server.o -lssl -lcrypto

run-server: server.o
	- ./server.o

run-client: client.py
	- python3 ./client.py

clear-server:
	- rm ./server.o

run-solver: solver.o
	- ./solver.o