# OpenHPC2-verify-MPI_Reduce
A test script to verify the successful implementation of a cluster (custom nodes/ranks) using MPI collectives (MPI_Reduce)

The default settings are for 4 processes, which is the test target for the OpenHPC 2.x virtual lab deployment.
https://hpc-ecosystems.gitlab.io/training/openhpc-2.x-guide/

ARRAY_SIZE of 4 is intended to address a Slurm job using -n 4 -N 2 (4 processes over 2 nodes).

To change the test parameters, update the ARRAY_SIZE. This was not designed to be a robust testing mechanism for clusters of different configurations and is explicitly designed to test 4 processes over 2 nodes - setting ARRAY_SIZE to 8 may not test 2 nodes with 4 cores if one compute node has, for instance, 8 cores.

This is a very naive implementation that does the job for 4 cores in 2 nodes - it will require some modifications to be more extensible.
