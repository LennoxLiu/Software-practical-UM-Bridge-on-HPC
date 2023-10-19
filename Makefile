all: clear build-load-balancer run-load-balancer

regular-server-obj = test/MultiplyBy2/minimal-server.cpp # specify your server here
slurm-server-obj = host-LoadBalancer.cpp LoadBalancer.hpp lib/httplib.h lib/json.hpp lib/umbridge.h

build-load-balancer:
	- g++ -O3 -Wno-unused-result -std=c++14 $(slurm-server-obj) -o load-balancer -pthread

build-regular-server: # use this if your server is a cpp file that needs to be compiled
	- g++ -O3 -w -std=c++11 $(regular-server-obj) -o server -pthread

run-load-balancer: load-balancer
	- mkdir -p ./urls
	- mkdir -p ./sub-jobs

	- ./load-balancer

run-client: client.py
	- python3 ./client.py

clear:
	- rm ./load-balancer
