#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:30:00
#SBATCH --mem=1gb

make clear-server
make build-server
host=$(srun hostname)
echo $host > hostname.txt
./server.o $host
rm hostname.txt
