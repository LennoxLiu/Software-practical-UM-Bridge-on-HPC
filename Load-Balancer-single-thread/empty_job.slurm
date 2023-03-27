#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=2
#SBATCH --time=00:10:00
#SBATCH --mem=1gb

@echo off
echo This is a batch script that will stay alive.
echo Press Ctrl+C to exit.

:loop
REM Your script code goes here

pause
goto loop