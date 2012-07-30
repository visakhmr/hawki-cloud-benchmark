#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#define N 1024
#define Log_N 15
#define cluster 4096
#define Log_P 3

int power(int i, int j){

/* Evaluates i^j */

  int k;
  int prod=1;

  for ( k = 0;k<j;k++)
    prod=prod*i;

  return prod;

}

int rev(int n){

/* Evaluates bit reverse of n taking base N/P */

  int i,m;

  m = 0;
  for (i=0;i<Log_N-Log_P;i++){
    m = m*2 + n%2;
    n = n/2;
  }

  return m;

}

void main(int argc, char **argv){

  int my_rank;
  int p;
  int source, dest;
  int tag = 50;
  MPI_Status status;
  int priority;
  double w[2],w_m[2],t[2],u[2];
  int m,m_red;
  double y[cluster][2],b[cluster][2];
  
  int i,k,j;
  int phase;
  MPI_Comm my_comm[Log_P];
  int my_pair;
  int rank[Log_P];
  int size;
  
/* Setting up of MPI communicators... */

  MPI_Init(&argc,&argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  printf("My rank %d\n",my_rank);

/* Forming Communicators...*/

  for (i=1;i<=Log_P;i++){
   m = power(2,i);
   priority = my_rank % m;
   if (priority < m/2) 
      my_pair=my_rank;
   else 
      my_pair= my_rank-m/2;
   MPI_Comm_split(MPI_COMM_WORLD, my_pair, my_rank, &my_comm[i-1]);
   MPI_Comm_rank(my_comm[i-1],&rank[i-1]);
  }

  for(i=0;i<Log_P;i++){
/* Testing size of each communicator */
    MPI_Comm_size(my_comm[i],&size);
    printf("proc %d size %d my_rank %d\n",my_rank,size,rank[i]);
  } 

  /* Initializing data */
  
  for (i=0;i<cluster;i++){
    y[i][0]=1;y[i][1]=0;
  }

  /* Reversing the data */

  for (i=0;i<cluster;i++){
    if (rev(i)<i){
      w[0]=y[i][0];
      w[1]=y[i][1];
      y[i][0]=y[rev(i)][0];
      y[i][1]=y[rev(i)][1];
      y[rev(i)][0]=w[0];
      y[rev(i)][1]=w[1];
    }
  }

  /* Calculating FFT locally */

  for (i=1;i<=Log_N-Log_P;i++){
    printf("Done round:%d of internal fft for proc %d\n",i,my_rank);
    m = power(2,i);
    w_m[0] = cos ((2 * M_PI)/m);
    w_m[1] = sin ((2 * M_PI)/m);
    w[0]=1;
    w[1]=0;
    for(j=0;j<=(m/2)-1;j++){
      for(k=j;k<=cluster-1;k+=m){
        t[0] = w[0]*y[k+m/2][0]-w[1]*y[k+m/2][1];
        t[1] = w[0]*y[k+m/2][1]+w[1]*y[k+m/2][0];
        u[0] = y[k][0]; u[1] = y[k][1];
        y[k][0] = u[0] + t[0];
        y[k][1] = u[1] + t[1];
        y[k+m/2][0] = u[0] - t[0];
        y[k+m/2][1] = u[1] - t[1];
      }
      w[0] = w[0]*w_m[0] - w[1]*w_m[1];
      w[1] = w[0]*w_m[1] + w[1]*w_m[0];
    }
  }

  /* Calculating the final FFT by communication and computation */

  for(i=1+Log_N-Log_P;i<=Log_N;i++){
    printf("Done round:%d of external fft for proc %d\n",i-Log_N+Log_P,my_rank);
    k=i-Log_N+Log_P-1;
    m = power(2,i);
    m_red = power(2,i-Log_N+Log_P);
    priority= my_rank % m_red;
    if ( priority < m_red/2){
      MPI_Send (y,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k]);
      MPI_Recv (b,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k],&status);
      for (j=0;j<cluster;j++){
        y[j][0] = y[j][0] + b[j][0];
        y[j][1] = y[j][1] + b[j][1];
      }
    }
    else 
      {
        w_m[0] = cos ((2 * M_PI)/m);
        w_m[1] = sin ((2 * M_PI)/m);
        w[0]=cos((2*M_PI*(priority-m_red/2))/m);;
        w[1]=sin((2*M_PI*(priority-m_red/2))/m);;
        
        for (j=0;j<cluster;j++){
          y[j][0] = y[j][0]*w[0]-y[j][1]*w[1];
          y[j][1] = y[j][0]*w[1]+y[j][1]*w[0];
          w[0] = w[0]*w_m[0] - w[1]*w_m[1];
          w[1] = w[0]*w_m[1] + w[1]*w_m[0];
        }

        MPI_Recv (b,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k],&status);
        MPI_Send (y,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k]);
        for (j=0;j<cluster;j++){
          y[j][0] = b[j][0] - y[j][0];
          y[j][1] = b[j][1] - y[j][1];
        }
        
      }
  }

/* Same stuff as above, but for calculating inverse FFT */

  for (i=0;i<cluster;i++){
    if (rev(i)<i){
      t[0]=y[i][0];
      t[1]=y[i][1];
      y[i][0]=y[rev(i)][0];
      y[i][1]=y[rev(i)][1];
      y[rev(i)][0]=t[0];
      y[rev(i)][1]=t[1];
    }
  }
  for (i=1;i<=Log_N-Log_P;i++){
    printf("Done round:%d of internal fft for proc %d\n",i,my_rank);
    m = power(2,i);
    w_m[0] = cos ((2 * M_PI)/m);
    w_m[1] = -1 *sin ((2 * M_PI)/m);
    w[0]=1;
    w[1]=0;
    for(j=0;j<=(m/2)-1;j++){
      for(k=j;k<=cluster-1;k+=m){
        t[0] = w[0]*y[k+m/2][0]-w[1]*y[k+m/2][1];
        t[1] = w[0]*y[k+m/2][1]+w[1]*y[k+m/2][0];
        u[0] = y[k][0]; u[1] = y[k][1];
        y[k][0] = u[0] + t[0];
        y[k][1] = u[1] + t[1];
        y[k+m/2][0] = u[0] - t[0];
        y[k+m/2][1] = u[1] - t[1];
      }
      w[0] = w[0]*w_m[0] - w[1]*w_m[1];
      w[1] = w[0]*w_m[1] + w[1]*w_m[0];
    }
  }
  for(i=1+Log_N-Log_P;i<=Log_N;i++){
    printf("Done round:%d of external fft for proc %d\n",i-Log_N+Log_P,my_rank);
    k = i - Log_N + Log_P - 1;
    m = power(2,i);
    m_red = power(2,i-Log_N+Log_P);
    priority= my_rank % m_red;
    if ( priority < m_red/2){
      MPI_Send (y,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k]);
      MPI_Recv (b,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k],&status);
      for (j=0;j<cluster;j++){
        y[j][0] = y[j][0] + b[j][0];
        y[j][1] = y[j][1] + b[j][1];
      }
    }
    else {
      w_m[0] = cos ((2 * M_PI)/m);
      w_m[1] = sin ((2 * M_PI)/m);
      w[0]=cos((2*M_PI*(priority-m_red/2))/m);;
      w[1]=sin((2*M_PI*(priority-m_red/2))/m);;
      
      for(j=0;j<cluster;j++){
        y[j][0] = y[j][0]*w[0]-y[j][1]*w[1];
        y[j][1] = y[j][0]*w[1]+y[j][1]*w[0];
        w[0] = w[0]*w_m[0] - w[1]*w_m[1];
        w[1] = w[0]*w_m[1] + w[1]*w_m[0];
      }

      MPI_Recv (b,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k],&status);
      MPI_Send (y,2*cluster,MPI_DOUBLE,1-rank[k],tag,my_comm[k]);
      for (j=0;j<cluster;j++){
        y[j][0] = b[j][0] - y[j][0];
        y[j][1] = b[j][1] - y[j][1];
      }
      
    }
  }
  
/* Doing validity check.... */

  for (j=0;j<cluster;j++)
   if (y[j][0]!=N ){
      printf("I did some mistake!! proc %d at %d with value %f\n",
           my_rank,j,y[j][0]);
      break;
   }

/* Outputing that the process is done */
   
  printf("I am done : proc %d\n", my_rank);
  MPI_Finalize();
} 

