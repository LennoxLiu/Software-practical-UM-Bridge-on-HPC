#!/bin/bash

# Define the range of ports to select from
MIN_PORT=1024
MAX_PORT=65535

# Generate a random port number
port=$(shuf -i $MIN_PORT-$MAX_PORT -n 1)

# Check if the port is in use
while lsof -Pi :$port -sTCP:LISTEN -t >/dev/null; do
    # If the port is in use, generate a new random port number
    port=$(shuf -i $MIN_PORT-$MAX_PORT -n 1)
done

echo $port