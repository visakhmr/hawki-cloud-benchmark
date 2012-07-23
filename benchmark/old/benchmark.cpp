// matrixstats.cpp
// Written by Drew Weitz
// Last Modified: June 9, 2003

// This program is intended to be a very basic introduction to MPI programming.
// For informationa bout each specific function call, consult the man pages
// available on the cluster, or check out information available online at
// www.iitk.ac.in/cc/param/mpi_calls.html

// to include the MPI function library

#include "mpi.h"
#include <stdio.h>
#include <string>

int main (int argc, char *argv[]) {

	// these variables will hold MPI specific information.
	// numprocs will store the total number of processors allocated
	// for the program run; myrank will store each individual procssors
	// number assigned by the master.

	int numprocs, myrank;

	// In general, all nodes have a copy of all the variables defined.

	// MPI_Init prepares the program run to communicate between all the
	// nodes.  It is necessary to have this function call in all MPI
	// code.

	MPI_Init(&argc, &argv);

	// MPI_Comm_size initializes the numprocs variable to be the number
	// of processors alloocated to the program run.  MPI_COMM_WORLD is
	// a macro that MPI uses to address the correct MPI netowrk.  Since
	// multiple MPI jobs could be running at once, we want to have a way
	// of addressing only the processors in our program run (or our 
	// "world").

	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	// MPI_Comm_rank initializes each nodes myrank variable to be it's
	// processor number.

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	MPI_Status status;
	char greeting[MPI_MAX_PROCESSOR_NAME + 80];

	int thematrix[numprocs-1][numprocs-1];
	int thecube[numprocs-1][numprocs-1][numprocs-1];
	int resultmatrix[numprocs-1][numprocs-1];
 	int temp  = 0;	
	if (myrank == 0) {

		printf("Preparing the matrix...\n");

		for (int i=0; i < numprocs-1; i++) {
			for (int j=0; j < numprocs-1; j++) {
				thematrix[i][j] = 0;
			}
		}

		printf("The matrix is:\n");
		for (int i=0; i < numprocs-1; i++) {
			for (int j=0; j < numprocs-1; j++) {
				printf("%i\t", thematrix[i][j]);
			}
			printf("\n");
		}

		printf("Sending the matrix to the other nodes...\n");

		for (int k=1; k < numprocs; k++) {

	// MPI_Send is the function call that sends the values of one nodes'
	// variable out to another node.  In effect, it is the way in which
	// data can be passed to another node.  A node can either receive this
	// "message" (via MPI_RECV), or it can be ignored.

			MPI_Send(thematrix, (numprocs-1)*(numprocs-1), 
					MPI_INT, k, 1, 
					MPI_COMM_WORLD);
		}

		printf("Waiting for responses...\n");
		for (int k=1; k < numprocs; k++) {

	// MPI_Recv is the compliament to MPI_Send.  This is one way
	// in which the processor will receive data.  In the case of
	// MPI_Recv, the processor will stop executing it's code and
	// wait for the expected message.  If you do not want the processor
	// to stop executing code while waiting for a "message", you must
	// use a "non-blocking" receive function.  See the website listed
	// at the tope of the program for more information.

			MPI_Recv(thematrix, (numprocs-1)*(numprocs-1), 
					MPI_INT, k, 1,
					MPI_COMM_WORLD, &status);
			for (int i=0; i < numprocs-1; i++) {
				for (int j=0; j < numprocs-1; j++) {
					thecube[k-1][i][j] = thematrix[i][j];
				}
			}
		}

		printf("The result is...\n");
		for (int k=0; k < numprocs-1; k++) {
			printf("Level %i:\n", k);
			for (int i=0; i < numprocs-1; i++) {
				for (int j=0; j < numprocs-1; j++) {
					printf("%i\t", thecube[k][i][j]);
				}
				printf("\n");
			}
		}
	
		printf("Collapsing results...\n");
		for (int i=0; i < numprocs-1; i++) {
			for (int j=0; j < numprocs-1; j++) {
				for (int k=0; k < numprocs -1; k++) {
					temp = temp + thecube [k][i][j];
				}
				resultmatrix[i][j] = temp;
				temp = 0;
			}
		}

		for (int i=0; i < numprocs-1; i++) {
			for (int j=0; j < numprocs-1; j++) {
				printf("%i\t", resultmatrix[i][j]);
			}
			printf("\n");
		}

	}
	
	else {

		MPI_Recv(thematrix, (numprocs-1)*(numprocs-1), MPI_INT, 0, 1, 
			MPI_COMM_WORLD, &status);
		sprintf(greeting, "matrix received...");
		for (int j=0; j < numprocs-1; j++) {
			thematrix[myrank-1][j] = myrank*j;
		}
		MPI_Send(thematrix, (numprocs-1)*(numprocs-1), MPI_INT, 0, 1, 
				MPI_COMM_WORLD);		
	}

	// MPI_FINALIZE closes all the connections opened during the MPI
	// program run.  It is necessary for MPI Programs.

	MPI_Finalize();
	return 0;
}
