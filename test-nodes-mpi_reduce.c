/*  Testing OpenHPC2.x virtual lab deployment
		https://hpc-ecosystems.gitlab.io/training/openhpc-2.x-guide/
    
    Run MPI collective/s to solve calculations that are dependant on the 
    correct parameters for the MPI job.

    1. MPI_Reduce to calculate a sum based on rank values for the communicator
       When -n 4 -N 2 is set, we will get a specific result of 100. 
			 Any other result is NOT compatible with '-n 4 -N 2' which is the prescribed configuration of the virtual lab.
			 In Slurm -N is equivalent to --nodes: the number of nodes (2)
			 In Slurm -n is equivalent to --ntasks: the number of processes (4)
*/

/*  MPI REDUCE TEST
 
    runs a reduction and produces different values depending on the number of ranks

		Using formula: (rank + 1) * (i + 1)

    salloc -n 4 -N 2 will produce:

		for i = 0 up to (world_size = 4)
    rank 0 ... (0+1)*1 	1*2 	1*3 	1*4 	==> 	1*1		1*2		1*3		1*4		==	10
    rank 1 ... (1+1)*1 	2*2 	2*3 	2*4 	==> 	2*1		2*2		2*3		2*4		==	20
    rank 2 ... (2+1)*1 	... 	3*3 	3*4 	==> 	3*1		3*2		3*3		3*4		==	30
    rank 3 ... (3+1)*1 	... 	4*3 	4*4 	==> 	4*1		4*2		4*3		4*4		==	40
    							10 + 	20 + 	30 + 	40																			100
						
		so the answer that validates 2 nodes / 4 cores working is 100 (10 20 30 40)

		try this with 2 nodes / 2 cores, or 1 node / 4 cores, etc. and the answer will not be 100!

		-n 1 -N 1 = 10						-n 1 -N 2 = nnodes set to 1 ==> 10		-n 1 -N 4 = nnodes set to 1 ==> 10
		-n 2 -N 2 = 30 (10+20)		-n 2 -N 4 = nnodes to 2 ==> 30 (10+20)
		-n 4 -N 1 = N/A						-n 4 -N 2 = 100

*/

#include <stdio.h>
#include <mpi.h>

#define ARRAY_SIZE 4 // we have 4 processes, over 2 nodes

int main(int argc, char **argv)
{
	int rank, size;
	int failed = 0; // return "failed state" at the end - return 0 (failed==false)
	int target = 0;

	// Initialize MPI
	MPI_Init(&argc, &argv);

	// Get the rank (ID) of the current process
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	// Get the total number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	// On each node, create an array to store unique values
	int node_array[ARRAY_SIZE];

	// Generate the prescribed values for each node, based on their rank in the MPI world
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		node_array[i] = (rank + 1) * (i + 1); // Unique values on each node
	}																				// for i upto ARRAY_SIZE

	// determine the target final answer
	if (rank == 0) {

		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			for (int j = 0; j < ARRAY_SIZE; j++)
			{
				target += (i + 1) * (j + 1);
			}
			// target += 10 * i; // 1+2+3+4 is 10, and it runs 10*1 + 10*2 + 10*3 + 10*4 ...
		}										// for ARRAY_SIZE downto i

		printf("Reduction Target is %d.\n", target);
	
	}

	// Perform a reduction operation (sum) across all nodes
	int global_sum[ARRAY_SIZE];
	MPI_Reduce(node_array, global_sum, ARRAY_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// Print results on the root node
	if (rank == 0)
	{
		int reduction = 0;
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			reduction += global_sum[i];
		}

		printf("Result is: %d.\n", reduction);

		if (reduction == target)
		{
			failed = 0; // failed state must be "false"
		}
		else
		{
			failed = 1;
		} // failed state is set to "true"

	if (failed == 1)
		printf("FAILURE! MPI_Reduction did not match expected results.\n");
		
	if (failed == 0)
		printf("SUCCESS! MPI_Reduction matched expected results.\n");

	}		// if (rank == 0)


	// Finalize MPI
	MPI_Finalize();

	return (failed);
}
