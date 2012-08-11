/*
 *  Simple molecular dynamics code.
 *  $Id: MD-c.c,v 1.2 2002/01/31 16:43:14 spb Exp spb $
 *
 * This program implements:
 *     long range inverse square forces between particles. F = G * m1*m2 / r**2
 *     viscosity term     F = -u V
 * If 2 particles approach closer than Size we flip the direction of the
 * interaction force to approximate a collision.
 *
 */
#include <stdio.h>
#include <math.h>
#include "coord.h"

void visc_force(int N,double *f, double visc, double *vel);
void add_norm(int N,double *r, double *delta);
double force(double W, double delta, double r);


void evolve(int count,double dt){
int  step;
int i,j,k,l;
double dr, dx[3];

/*
 * Loop over timesteps.
 */
      for(step = 1;step<=count;step++){
        printf("timestep %d\n",step);
        printf("collisions %d\n",collisions);

/* set the viscosity term in the force calculation */
        for(i=0;i<Nbody;i++){
          visc_force(Ndim,particle[i].f, particle[i].visc, particle[i].vel);
/* calculate norm of seperation vector */
          particle[i].r = 0.0;
	  add_norm(Ndim,&particle[i].r,particle[i].pos);
          particle[i].r = sqrt(particle[i].r);
       /* calculate central force */
	  for(l=0;l<Ndim;l++){
                particle[i].f[l] = particle[i].f[l] - 
                   force(G*particle[i].mass*M_central,particle[i].pos[l],particle[i].r);
	  }
	}

/* calculate pairwise separation of particles */
        for(i=0;i<Nbody;i++){
          for(j=i+1;j<Nbody;j++){
         
	   for(l=0;l<Ndim;l++){
              dx[l] = particle[i].pos[l] - particle[j].pos[l];
            }
            dr = 0.0;
	    add_norm(Ndim, &dr, dx);
            dr = sqrt(dr);

            for(l=0;l<Ndim;l++){
/*  flip force if close in */
              double force1 = force(G*particle[i].mass*particle[j].mass,dx[l],dr);
              if( dr >= Size ){
                particle[i].f[l] = particle[i].f[l] - force1;
                particle[j].f[l] = particle[j].f[l] + force1;
              }else{
                particle[i].f[l] = particle[i].f[l] + force1;
                particle[j].f[l] = particle[j].f[l] - force1;
		collisions++;
              }
            }
          }
        }

/* update positions */
        for(i=0;i<Nbody;i++){
          for(j=0;j<Ndim;j++){
            particle[i].pos[j] = particle[i].pos[j] + dt * particle[i].vel[j];
/* update velocities */
            particle[i].vel[j] = particle[i].vel[j] + dt * (particle[i].f[j]/particle[i].mass);
          }
        }
      }
}




