# Software practical UM-Bridge on HPC

## Overall goal

Allow any UM-Bridge client to control models running on an HPC cluster. Therefore build an UM-Bridge server that, instead of doing work itself, submits SLURM jobs. These jobs should run the desired UM-Bridge model. The model should ideally receive parameters as usual.

UQ client <-> UM-Bridge server <-> SLURM <-> UM-Bridge Model in SLURM job

Main question: How to pass parameters from UM-Bridge server to model running inside SLURM job? (Can we do direct network communication (would be ideal)? Can we pass large amounts of data as CLI arguments to SLURM jobs? Should we use files on a shared filesystem instead?)

## Next steps

- Build a basic UM-Bridge server/model that, instead of computing something simple like in the examples, calls a bash command and returns some dummy value. We can later extend this to call SLURM instead.
- Read up on SLURM. There are many good tutorials online. Particularly relevant for you:
    - SLURM job files and what their options mean
    - Submitting jobs to SLURM (sbatch command etc.)
    - Monitoring running jobs (squeue command etc.)

## Processing & questions

- Familiar with Helix, E-learning module: [Introduction to bwForCluster Helix](https://training.bwhpc.de/goto.php?target=crs_695&client_id=BWHPC)
- Working on parameter passing C++ UM-Bridge server

## Done

- [2023-03-09] Register Slack account
- [2023-03-10] Register Helix account
> Get access to Helix cluster https://wiki.bwhpc.de/e/Registration/bwForCluster (You can join our existing Rechenvorhaben(RV)/project. I will add you once you have completed step A.))
