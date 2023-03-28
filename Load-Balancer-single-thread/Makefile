all: clear-server build-server run-server

regular-server-obj =  ./eigen/minimal-server.cpp ./eigen/solver.hh ./eigen/matrix.hh ./eigen/matrix.cc
slurm-server-obj = test-server.cpp load-balancer.hpp ../lib/httplib.h ../lib/json.hpp ../lib/umbridge.h

build-load-balancer:
	- g++ -O3 -Wall -std=c++11 $(slurm-server-obj) $(regular-server-obj) -o load-balancer.o -lssl -lcrypto -pthread

build-regular-server:
	- g++ -O3 -w -std=c++11 $(regular-server-obj) -o server.o -lssl -lcrypto -pthread

build-server:
	- make build-load-balancer
	- make build-regular-server

run-server: load-balancer.o
	- ./load-balancer.o

run-client: client.py
	- python3 ./client.py

clear-server:
	- rm ./load-balancer.o
	- rm ./server.o

helix-make: helix-build
	- make run-server

helix-build:
	- git reset --hard
	- git pull
	- make build-server

helix-update:
	- git reset --hard
	- git pull
