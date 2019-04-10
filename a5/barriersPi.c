//Computes an estimate for pi in parallel using pthreads.
//Whenever a thread computes a value, it adds its contributions into a global array sum
//At the end, the main thread adds the values in the array to produce an estimate for pi.
//Instead of using pthread_join, this program uses barriers to synchronize.

#include <pthread.h> /*used in other parts of the assignment */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */
//#include <atomic>    /*used in other parts of the assignment */

#define MAX_THREADS 8
pthread_t handles[MAX_THREADS];
pthread_barrier_t barr;
int shortNames[MAX_THREADS];

void *compute_pi (void *);

int numPoints;
int NUM_THREADS;
double step;
double pi;
double sum[MAX_THREADS];
double x = 0.0d;

double f(double x) {
  return (6.0/sqrt(1-x*x));
}

int main(int argc, char *argv[]) {
  pthread_attr_t attr;
  pthread_attr_init (&attr);
  pthread_barrier_init(&barr, NULL, atoi(argv[1]));

  pi = 0.0;
  numPoints = 1000000000;
  step = 0.5/numPoints;
  NUM_THREADS = atoi(argv[1]); //number of threads is an input

  uint64_t execTime; /*time in nanoseconds */
  struct timespec tick, tock;
  clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
  
  //create threads
  for(int i = 0; i < NUM_THREADS; i++)
  {
    //printf("Creating thread %d\n", i);
    shortNames[i] = i;
    pthread_create(& handles[i], &attr, compute_pi, & shortNames[i]);
  }

  pthread_barrier_wait(&barr);
  for(int i = 0; i < NUM_THREADS; i++) //only to add contributions, not to synchronize threads.
  {
    pi += sum[i];
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
 
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
 
  printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) execTime);

  printf("%.20f\n", pi);
  return 0;
}

//function that computes pi using threads and adding contributions to an array
void *compute_pi (void *threadIdPtr)
{
  double mySum = 0.0;
  int myId = *(int *)threadIdPtr;

  for(int i = myId; i < numPoints; i+=NUM_THREADS)
  {
    x = step * ((double) i); //next x
    mySum += step * f(x); //add contributions to local variable
  }

  sum[myId] = mySum; //write final contribution to global sum array
  pthread_barrier_wait(&barr);
}