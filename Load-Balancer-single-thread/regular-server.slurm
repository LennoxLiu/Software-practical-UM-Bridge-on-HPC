#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:10:00
#SBATCH --mem=1gb

echo $(./start_regular_server.sh server.o)
