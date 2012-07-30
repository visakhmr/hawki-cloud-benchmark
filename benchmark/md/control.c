/*
 * Control program for the MD update
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#define DECL
#include "coord.h"


double second()
{
        struct timeval tp;
        struct timezone tzp;
        int i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


int main(int argc, char *argv){
  int i,j;
  FILE *in, *out;
  char name[80];  
  double  t0, t1;	    /* wall time variables */
  clock_t c0, c1; /* clock_t is defined on <bits/types.h> as long */
  char hostname[100]; 
  double dt=0.02;  /*  timestep value */
  int Nstep=1;
  gethostname(hostname,100);
/*  number of timesteps to use. */
  
/* read the initial data from a file */
  collisions=0;
  in = fopen("input.dat","r");

  if( ! in ){
    perror("input.dat");
    exit(1);
  }

  for(i=0;i<Nbody;i++){
    fscanf(in,"%16le%16le%16le%16le%16le%16le%16le%16le\n",&particle[i].mass,&particle[i].visc, &particle[i].pos[Xcoord], &particle[i].pos[Ycoord], &particle[i].pos[Zcoord], &particle[i].vel[Xcoord], &particle[i].vel[Ycoord], &particle[i].vel[Zcoord]);
  }
  fclose(in);

/*
 * Run Nstep timesteps and time how long it took
 */
  t0 = second();
  c0 = clock();

  evolve(Nstep,dt); 

  t1 = second();
  c1 = clock();
  printf ("\nExecution..Host: %s// Clock time: %f seconds // CPU time: %f\n", hostname, (float) (t1 - t0), (float) (c1 - c0)/CLOCKS_PER_SEC);
 
/* WRITE final result to a file */
      sprintf(name,"output.dat%03d",j*Nstep);
      out = fopen(name,"w");

      if( ! out ){
	perror(name);
	exit(1);
      }

      for(i=0;i<Nbody;i++){
	fprintf(out,"%16.8E%16.8E%16.8E%16.8E%16.8E%16.8E%16.8E%16.8E\n",
		particle[i].mass,particle[i].visc,
		particle[i].pos[Xcoord], particle[i].pos[Ycoord], particle[i].pos[Zcoord],
		particle[i].vel[Xcoord], particle[i].vel[Ycoord], particle[i].vel[Zcoord]);
      }
      fclose(out);
}

