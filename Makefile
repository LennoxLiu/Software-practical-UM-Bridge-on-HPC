all: clear build-load-balancer run-load-balancer

regular-server-obj = test/debug/debug-server.cpp
slurm-server-obj = host-LoadBalancer.cpp LoadBalancer.hpp lib/httplib.h lib/json.hpp lib/umbridge.h

build-load-balancer:
	- g++ -O3 -Wno-unused-result -std=c++14 $(slurm-server-obj) -o load-balancer -lssl -lcrypto -pthread

build-regular-server:
	- g++ -O3 -w -std=c++11 $(regular-server-obj) -o server -lssl -lcrypto -pthread

run-load-balancer: load-balancer
	- mkdir -p ./urls
	- mkdir -p ./sub-jobs

	- ./load-balancer

run-client: test/debug/debug-client.py
	- python3 test/debug/debug-client.py

clear:
	- rm ./load-balancer
