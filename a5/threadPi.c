#include <pthread.h> /*used in other parts of the assignment */
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */
//#include <atomic>    /*used in other parts of the assignment */

#define MAX_THREADS 8
pthread_t handles[MAX_THREADS];
int shortNames[MAX_THREADS];

void *compute_pi (void *);

int numPoints;
int NUM_THREADS;
double step;
double pi;
double x = 0.0d;

double f(double x) {
  return (6.0/sqrt(1-x*x));
}

int main(int argc, char *argv[]) {
  pthread_attr_t attr;
  pthread_attr_init (&attr);

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
    shortNames[i] = i;
    pthread_create(& handles[i], &attr, compute_pi, & shortNames[i]);
  }

  //join with threads. No need to add contributions since they were directly added to the global variable
  for(int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(handles[i], NULL);
  }

  clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
 
  execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
 
  printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) execTime);

  printf("%.20f\n", pi);
  return 0;
}

//function that computes pi using threads and adding directly to a global variable pi
void *compute_pi (void *threadIdPtr)
{
  int myId = *(int *)threadIdPtr;

  for(int i = myId; i < numPoints; i+=NUM_THREADS)
  {
    x = step * ((double) i); //next x
    pi += step * f(x);
  }
}