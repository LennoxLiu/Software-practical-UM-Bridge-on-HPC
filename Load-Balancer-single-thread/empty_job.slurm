#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=2
#SBATCH --time=00:30:00
#SBATCH --mem=1gb

echo This is a batch script that will stay alive to take commands.
echo Press Ctrl+C to exit.

while true; do
    read command
    $command
done