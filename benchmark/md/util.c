#include <math.h>

void visc_force(int N,double *f, double visc, double *vel)
{
  int i;
          for(i=0;i<N;i++){
            f[i] = -visc * vel[i];
          }
}
void add_norm(int N,double *r, double *delta)
{
  int k;
        for(k=0;k<N;k++){
          *r += (delta[k] * delta[k]);
        }
}

double force(double W, double delta, double r){
  return W*delta/(double)(r*r*r);
}



