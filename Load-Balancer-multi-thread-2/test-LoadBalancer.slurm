#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:30:00
#SBATCH --mem=1gb

module load compiler/gnu
make clear-server
make build-server
make run-server

