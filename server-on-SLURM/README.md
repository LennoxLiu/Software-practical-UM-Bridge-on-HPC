# README

## How to start server on Helix

1. Load module to compile `cpp` files: `module load compiler/gnu`

2. (Optional) Set the port of server: `export PORT=4243`

    > Sometimes the default port 4242 of the login node is occupied.

3. Compile and run the server
    - Build and run on login node: `make helix-make`
    
    > Pull from the repo and build the server: `make helix-build`
    
    - Run on computing node: `sbatch test-server.slurm`

## How to run client on Helix

1. Create a `conda` environment with `umbridge` package. Name it as `umbridge`.

2. Load `conda` environment whenever login to a new node.
	```
    module load devel/miniconda/3
    source $MINICONDA_HOME/etc/profile.d/conda.sh
    conda activate umbridge
   ```

3. Run client.py on login node: `python3 client.py`