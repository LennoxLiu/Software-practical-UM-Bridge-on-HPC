all: build-server run-server

objects2 = minimal-server.cpp httplib.h json.hpp umbridge.h solver.hh matrix.hh matrix.cc
build-server:
	- g++ -O3 -w -std=c++11 $(objects2) -o server.o -lssl -lcrypto

run-server: server.o
	- ./server.o

run-client: client.py
	- python3 ./client.py

clear-server:
	- rm ./server.o

objects = solver.cc matrix.hh matrix.cc
build-solver: $(objects)
	- g++ -O3 -Wall -std=c++11 $(objects) -o solver.o

run-solver: solver.o
	- ./solver.o