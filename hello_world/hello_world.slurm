#!/bin/bash

#SBATCH --partition=single
#SBATCH --ntasks=1
#SBATCH --time=00:05:00
#SBATCH --mem=500mb

g++ hello_world.cpp -o hello-slurm.o
./hello-slurm.o

