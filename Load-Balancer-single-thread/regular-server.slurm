#!/bin/bash
#SBATCH --partition=devel
#SBATCH --ntasks=1
#SBATCH --time=00:10:00
#SBATCH --mem=1gb

rm url.txt

function get_avaliable_port {
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
}

server_file="./server.o"
port=$(get_avaliable_port)

hostname= $(hostname)
echo $hostname

export PORT=$port

# run the server, server should listen to hostname instead of localhost or 0.0.0.0
echo " "$($server_file)" | grep -o 'Listening on port [^ ]*' | sed 's/Listening on port //'" > url.txt

