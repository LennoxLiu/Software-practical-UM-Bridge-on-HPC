# README

To allow any UM-Bridge client to control models running on an HPC cluster.

## File descriptions

- `LoadBalancer.hpp`

  The main header file that implement the LoadBalancer server as a C++ class `LoadBalancer`. To use this project, you need to include this file in your program.

- `LoadBalancer.slurm`

  A slurm configuration file, which is used to start a LoadBalancer server in a computing node

- `regular-server.slurm`

  A slurm configuration file, which is used to start a slurm job running a rugular server in a computing node

- `host-LoadBalancer.cpp`

  An example about how to use the class LoadBalancer

## How to start a LoadBalancer server

>The LoadBalancer server is supposed to run at login node, but it can also run at computing node.

1. Load module that is necessary to compile `cpp` files
> e.g. On Helix it's `module load compiler/gnu`

2. (**Optional**) Set the port of server: `export PORT=4243`
> Sometimes the default port 4242 of the login node is occupied.

3. Compile and run the server
    > `LoadBalancer.hpp` is the file you need to include to use the LoadBalancer. Note that this file will include `umbridge.hpp`.

    - Compile the LoadBalancer server: `make build-load-balancer`
    > Modify the variable `slurm-server-obj` in the file `Makefile` if you want to use the class LoadBalancer in your own way.

    - Load a regular server. Specify the path of your server file in file `regular-server.slurm`, as the variable `server_file`.
    > You can also specify slurm parameters in the file `regular-server.slurm`.
    - Run the LoadBalancer server: `make run-load-balancer`

    > You can specify slurm parameters in the file `LoadBalancer.slurm`
    > The the LoadBalancer server will occupy a terminal, so you need to start a new one if you want to run a client on the same node.

> The Load Balancer will submit a new SLURM job whenever it receives an evaluation request, and cancel the SLURM job when the evaluation is finished.
> The Load Balancer will listen to the hostname of node instead of localhost.
> The regular server in SLURM job will also listen to the hostname and use a random port that is not in use.

## How to connect a client to the LoadBalancer server

1. Activate a python environment on the cluster that contains `umbridge` python package
>e.g. On Helix:
>
>1.1 Create a `conda` environment with `umbridge` package.
>
>1.2 Load `conda` environment whenever login to a new node.
>```
>    module load devel/miniconda/3
>    source $MINICONDA_HOME/etc/profile.d/conda.sh
>    conda activate umbridge
>```

2. Run the client: `python3 client.py <LoadBalancer server hostname>`
> A client is supposed to run on the login node or at your own device, since it does not perform intensive calculations.
> You can get the hostname of LoadBalancer server from hostname.txt, output by the LoadBalancer server.

## How to run a client on laptop

1. Create a SSH tunnel to the LoadBalancer server
>e.g. On Helix:
>
>```
>    ssh <username>@helix.bwservices.uni-heidelberg.de -N -f -L <local port>:<server hostname>:<server port>
>    # start ssh tunnel
>    # -N : do not execute remote command
>    # -f : request ssh to go to the background once the ssh connection has been established
>```

2. (**Optional**) Set a port on laptop for the client to use: `export PORT=<local port>`

3. Run client at localhost: `python3 client.py localhost`

## Example

An example server is in the folder `test/MultiplyBy2`. The server `minimal-server.cpp` take the input written in `client.py`, multiply them by 2 and then return.

Currently, it will run and test 4 models in parallel, but the LoadBalancer server will process them in sequence.