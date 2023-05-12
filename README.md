# README

To allow any UM-Bridge client to control models running on an HPC cluster.

## How to start server on Helix

1. Load module to compile `cpp` files: `module load compiler/gnu`

2. (**Optional**) Set the port of server: `export PORT=4243`

    > Sometimes the default port 4242 of the login node is occupied.

3. Compile and run the server
    - Pull from the repo and build the server: `make helix-build`
    
    - Run on computing node: `make run-server`

> The Load Balancer will submit a new SLURM job whenever it receives an evaluation request, and cancel the SLURM job when the evaluation is finished.
> The Load Balancer will listen to the hostname of node instead of localhost.
> The regular server in SLURM job will also listen to the hostname and use a random port that is not in use.

## How to run client on Helix

1. Create a `conda` environment with `umbridge` package. Name it as `umbridge`.

2. Load `conda` environment whenever login to a new node.
	```
    module load devel/miniconda/3
    source $MINICONDA_HOME/etc/profile.d/conda.sh
    conda activate umbridge
   ```

3. Run client.py on login node: `python3 client.py <server hostname>`

> Read hostname from hostname.txt output by the server SLURM job.

## How to run client on laptop

1. Create a SSH tunnel:

    ```
    ssh <username>@helix.bwservices.uni-heidelberg.de -N -f -L <local port>:<server hostname>:<server port>
    # start ssh tunnel
    # -N : do not execute remote command
    # -f : request ssh to go to the background once the ssh connection has been established
    ```

2. Set port on laptop: `export PORT=<local port>`

3. Run client at localhost: `python3 client.py localhost`

