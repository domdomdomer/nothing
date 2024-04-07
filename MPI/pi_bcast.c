#include <stdio.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char **argv) 
{
	int my_id, root_process, num_procs, ierr;
	double term,pi;
	MPI_Status status;      

	root_process = 0;      

	ierr = MPI_Init(&argc, &argv);

	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	term=0;

	ierr = MPI_Bcast(&term, 1, MPI_DOUBLE, root_process,MPI_COMM_WORLD);
	
	if(my_id%2==0)
	{
		for(int i=0;i<10;i++)
		{
			term+=(double)4/(double)(2*(my_id+num_procs*i)+1);
		}
		
	}
	else{
		
		for(int i=0;i<10;i++)
		{
			term+=(double)-4/(double)(2*(my_id+num_procs*i)+1);
		}

	}
	
	
	ierr = MPI_Reduce(&term, &pi, 1, MPI_DOUBLE,MPI_SUM, root_process, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	/* and, if I am the root process, print the result. */

	if(my_id == root_process) {
		printf("The value of pi is %f\n", pi);

		
	} 

	ierr = MPI_Finalize();
}     
