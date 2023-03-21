all: clear-server build-server run-server

objects2 = test-server.cpp slurm-server.hpp ../lib/httplib.h ../lib/json.hpp ../lib/umbridge.h
build-server:
	- g++ -O3 -w -std=c++11 $(objects2) -o server.o -lssl -lcrypto -pthread

run-server: server.o
	- ./server.o

run-client: client.py
	- python3 ./client.py

clear-server:
	- rm ./server.o

helix-make:
	- git reset --hard
	- git pull
	- make build-server
	- make run-server

helix-update:
	- git reset --hard
	- git pull

load-conda:
	- module load devel/miniconda/3
	- source ${MINICONDA_HOME}/etc/profile.d/conda.sh
	- conda activate umbridge