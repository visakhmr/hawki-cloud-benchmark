/*
 * $Id: control-c.c,v 1.2 2002/01/08 12:32:48 spb Exp spb $
 *
 * Control program for the MD update
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define DECL
#include "coord.h"

double second(void); 
int main(int argc, char *argv[]){
  int i,j;
FILE *in, *out;
double start,stop;
 char name[80];
/*  timestep value */
double dt=0.02;

/*  number of timesteps to use. */
int Nstep=100;
 int Nsave=5;
  if( argc > 1 ){
   Nstep=atoi(argv[1]);
  }


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

  for(j=1;j<=Nsave;j++){
      start=second();
      evolve(Nstep,dt); 
      stop=second();


      printf("%d timesteps took %f seconds\n",j*Nstep,stop-start);
      printf("collisions %d\n",collisions);

/* write final result to a file */
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

}

double second()
{
/* struct timeval { long        tv_sec; 
            long        tv_usec;        };

struct timezone { int   tz_minuteswest;
             int        tz_dsttime;      };     */

        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

