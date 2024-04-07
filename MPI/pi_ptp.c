#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define data_tag 2001

int main(int argc, char **argv) 
{
	int my_id, root_process, num_procs, ierr;
	double term,pi,term1;
	MPI_Status status;      

	root_process = 0;      

	ierr = MPI_Init(&argc, &argv);

	ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
	term=0;

	if(my_id==0)
	{
		term1=0;
		for(int i=1;i<num_procs;i++){
			ierr=MPI_Recv(&term1,1,MPI_DOUBLE,i,data_tag,MPI_COMM_WORLD,&status);
			pi+=term1;
		}
		for(int i=0;i<10;i++)
		{
			pi+=(double)4/(double)(2*(my_id+num_procs*i)+1);
		}
		printf("The value of pi is %f\n", pi);
	}
	else{
		term=0;
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
		ierr=MPI_Send(&term,1,MPI_DOUBLE,0,data_tag,MPI_COMM_WORLD);
		
	}
	ierr = MPI_Finalize();
}     
