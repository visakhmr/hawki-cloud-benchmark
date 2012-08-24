/*
 * Control program to update 
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
  double  t0, t1;	    /* wall time variables */
  double dt=0.02;  /*  timestep value */
  int Nstep=1; /*One step for benchmark*/
  collisions=0;

/*Generate input, Assign dummy values*/
  for(i=0;i<Nbody;i++){
    	particle[i].mass=1.200000;
	particle[i].visc=0.010000;
	particle[i].pos[Xcoord]=1999.997600;
	particle[i].pos[Ycoord]=3.067960;
	particle[i].pos[Zcoord]=0.000000;
	particle[i].vel[Xcoord]=0.001534;
	particle[i].vel[Ycoord]=-0.999999;
	particle[i].vel[Zcoord]=0.000000;
  }

/*
 * Run Nstep timesteps and time how long it took
 */
  t0 = second();
  evolve(Nstep,dt); 
  t1 = second();
  printf ("\nExecution time: %f seconds \n", (float) (t1 - t0));
       //DO NOT REMOVE THIS STATUS MESSAGE
        printf("\nstatus success\n");
        return 0; 
}

