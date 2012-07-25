/*----------------------------------------------
   fft_test.c - demonstration program for fft.c
  ----------------------------------------------*/

/******************************************************************************
 * This program demonstrates how to use the file fft.c to calculate an FFT    *
 * of given time-domain samples, as well as to calculate an inverse FFT       *
 * (IFFT) of given frequency-domain samples.  First, N complex-valued time-   *
 * domain samples x, in rectangular form (Re x, Im x), are read from a        *
 * specified file; the 2N values are assumed to be separated by whitespace.   *
 * Then, an N-point FFT of these samples is found by calling the function     *
 * fft, thereby yielding N complex-valued frequency-domain samples X in       *
 * rectangular form (Re X, Im X).  Next, an N-point IFFT of these samples is  *
 * is found by calling the function ifft, thereby recovering the original     *
 * samples x.  Finally, the calculated samples X are saved to a specified     *
 * file, if desired.                                                          *
 ******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "fft.c"
#include "sys/types.h"
#include "sys/sysinfo.h"




struct CpuInfo {
	char vendor_id[50];
	int family;
	char model[50];
	float freq;
	char cache[20];
};

void cpuinfo()
{
	struct CpuInfo info = {"", 0, "", 0.0, ""};
	
	FILE *cpuInfo;
	
	if( ( (cpuInfo = fopen("/proc/cpuinfo", "rb")) == NULL ) ) {
		printf("ERRORE! Impossibile aprire il file relativo alla CPU.");
		}
	else {
		while (!feof(cpuInfo)) {
			fread(&info, sizeof(struct CpuInfo), 1, cpuInfo);
			
			if(info.family !=0) {
				printf("%s\n%d\n%s\n%.2f\n%s\n", info.vendor_id, info.family, info.model, info.freq, info.cache);
				}
			}
		}
}




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
  int N;                    /* number of points in FFT */
  double (*x)[2];           /* pointer to time-domain samples */
  double (*X)[2];           /* pointer to frequency-domain samples */
  double dummy;             /* scratch variable */
  FILE *fp;                 /* file pointer */
  double  t0, t1; /* time_t is defined on <bits/types.h> as long */
  clock_t c0, c1; /* clock_t is defined on <bits/types.h> as long */
  struct sysinfo memInfo;
  char hostname[100];

  gethostname(hostname,100);
  cpuinfo();
  /* Get name of input file of time-domain samples x. */
  strcpy(file, "data.txt");

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
  c0 = clock();

  for(i=0; i<N; i++) fscanf(fp, "%lg%lg", &x[i][0], &x[i][1]);
  fclose(fp);
  
  t1 = second();
  c1 = clock();
  printf ("\n\tFile read..Host: %s// Clock time: %f seconds // CPU time: %f", hostname, (float) (t1 - t0), (float) (c1 - c0)/CLOCKS_PER_SEC);

  // Calculate FFT
  t0 = second();
  c0 = clock();
  fft(N, x, X);
  t1 = second();
  c1 = clock();
  printf ("\n\tExecution..Host: %s// Clock time: %f seconds // CPU time: %f", hostname, (float) (t1 - t0), (float) (c1 - c0)/CLOCKS_PER_SEC);

  // Free memory
  free(x);
  free(X);

  return 0;
}



