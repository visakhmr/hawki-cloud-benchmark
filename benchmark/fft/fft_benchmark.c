/*******************************

Sample FFT micro benchmark program
inputfile should be given as command line argument
eg: ./benchmark data.txt

*******************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "fft.c"
#include "sys/types.h"
#define REAL 3.2
#define IMAGINARY 4.6

double second()
{
        struct timeval tp;
        struct timezone tzp;
        int i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

int main(int argc, char **argv)
{
  	int i;                    /* generic index */
 	long long N;                    /* number of points in FFT */
  	double (*x)[2];           /* pointer to time-domain samples */
  	double (*X)[2];           /* pointer to frequency-domain samples */
  	double dummy;             /* scratch variable */
  	double  t0, t1;	    /* wall time variables */
 	clock_t c0, c1; /* clock_t is defined on <bits/types.h> as long */

	/* Get input size of time-domain samples x. */
	N= atoi(argv[1]);
	
  	/* Check that N = 2^n for some integer n >= 1. */
  	if(N >= 2)
    	{
      		i = N;
      		while(i==2*(i/2)) i = i/2;  // While i is even, factor out a 2. 
    	}  
	// For N >=2, we now have N = 2^n iff i = 1
  	if(N < 2 || i != 1)
    	{
      		printf(", which does not equal 2^n for an integer n >= 1.");
      		exit(EXIT_FAILURE);
    	}

  	// Allocate time- and frequency-domain memory
  	x = malloc(2 * N * sizeof(double));
  	X = malloc(2 * N * sizeof(double));

  	// Get time-domain samples
  	for(i=0; i<N; i++) 
  	{
		x[i][0]=REAL;
		x[i][1]=IMAGINARY;
  	}
  
  	// Calculate FFT
  	t0 = second();
  	fft(N, x, X);
  	t1 = second();
  	printf ("\nFFT Computation time=%f", (float) (t1 - t0));

  	// Free memory
  	free(x);
  	free(X);

	//DO NOT REMOVE THIS STATUS MESSAGE
  	printf("\nstatus success\n");
  	return 0;
}
