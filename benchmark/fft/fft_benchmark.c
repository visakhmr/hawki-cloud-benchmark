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
  char file[FILENAME_MAX];  /* name of data file */
  char output[FILENAME_MAX];/* name of data file */
  int N;                    /* number of points in FFT */
  double (*x)[2];           /* pointer to time-domain samples */
  double (*X)[2];           /* pointer to frequency-domain samples */
  double dummy;             /* scratch variable */
  FILE *fp;                 /* file pointer */
  double  t0, t1;	    /* wall time variables */
  clock_t c0, c1; /* clock_t is defined on <bits/types.h> as long */
  char hostname[100];

  gethostname(hostname,100);
//  cpuinfo();
  /* Get name of input file of time-domain samples x. */
  strcpy(file, argv[1]);
  strcpy(output, "output.txt");

  /* Read through entire file to get number N of points in FFT. */
  if(!(fp = fopen(file, "r")))
    {
      printf("   File \'%s\' cannot be opened", file);
      exit(EXIT_FAILURE);
    }
  N=0;
  while(fscanf(fp, "%lg%lg", &dummy, &dummy) == 2) N++;
  printf("N = %d", N);

  /* Check that N = 2^n for some integer n >= 1. */
  if(N >= 2)
    {
      i = N;
      while(i==2*(i/2)) i = i/2;  // While i is even, factor out a 2. 
    }  // For N >=2, we now have N = 2^n iff i = 1
  if(N < 2 || i != 1)
    {
      printf(", which does not equal 2^n for an integer n >= 1.");
      exit(EXIT_FAILURE);
    }

  // Allocate time- and frequency-domain memory
  x = malloc(2 * N * sizeof(double));
  X = malloc(2 * N * sizeof(double));

  // Get time-domain samples
  rewind(fp);

  t0 = second();
  for(i=0; i<N; i++) fscanf(fp, "%lg%lg", &x[i][0], &x[i][1]);
  fclose(fp);
  
  t1 = second();
  printf ("\nreadtime=%f", (float) (t1 - t0));

  // Calculate FFT
  t0 = second();
  fft(N, x, X);
  t1 = second();
  printf ("\ncomputationtime=%f", (float) (t1 - t0));

  // Write frequency domain samples
  if(!(fp = fopen(output, "w")))
  {
  	printf("   File \'%s\' could not be opened!", file);
	exit(EXIT_FAILURE);
  }

  t0 = second();
  for(i=0; i<N; i++) fprintf(fp, "%23.15e  %23.15e\n", X[i][0], X[i][1]);
  fclose(fp);
  t1 = second();
  printf ("\nwritetime%f", (float) (t1 - t0));

  // Free memory
  free(x);
  free(X);
  printf("\nstatus sucess\n");

  return 0;
}
