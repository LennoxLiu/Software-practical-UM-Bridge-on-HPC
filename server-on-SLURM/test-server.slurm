#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:05:00
#SBATCH --mem=500mb
#SBATCH --export=None

make build-server
make run-server
