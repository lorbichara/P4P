// Lorraine Bichara - lb34995
// CS 377P
// february 6, 2019

#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

int main() {

	//CPUID
	int a, b;
	__asm__("cpuid"
			:"=a"(b)
			:"0"(a)
			:"%ebx","%ecx","%edx");

	// clean cache by reading a lot of unuseful data
	const int size = 20*1024*1024; // Allocate 20M. Set much larger then L2
	char *c = (char *)malloc(size);
	for (int i = 0; i < 0xffff; i++)
	{
		for (int j = 0; j < size; j++)
		{
			c[j] = i*j;
		}
	}

	// ask the user for matrix dimension 
	int d;
	printf("Matrix dimension? ");
	scanf("%d", &d);

	// create matrices using malloc so they're big enough
	double ** matA;
	double ** matB;
	double ** result;

	matA = (double**)malloc(sizeof(double*)*d);
	for(int i = 0; i < d; i++)
		matA[i] = (double*)malloc(sizeof(double)*d);

	matB = (double**)malloc(sizeof(double*)*d);
	for(int i = 0; i < d; i++)
		matB[i] = (double*)malloc(sizeof(double)*d);

	result = (double**)malloc(sizeof(double*)*d);
	for(int i = 0; i < d; i++)
		result[i] = (double*)malloc(sizeof(double)*d);

	// initialize matrix A with random doubles
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			double range = (100-1);
			double div = RAND_MAX / range;
			matA[i][j] = 1 + (rand() / div);
		}
	}

	// initialize matrix B with random doubles
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			double range = (100-1);
			double div = RAND_MAX / range;
			matB[i][j] = 1 + (rand() / div);
		}
	}

	// ask the user for the desired method to take measurements
	char m;
	printf("Do you want to execution time measurements using PAPI (p) or clock_gettime (c)? ");
	scanf(" %c", &m);

	if(m == 'p') // PAPI
	{	
		// this is to measure execution time:
		float real_time, proc_time, mflops;
		long long flpins;
		int execTime;
		execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

		//long long counters[2];

		// int PAPI_events[] = {
		// 	PAPI_TOT_CYC,
		// 	PAPI_TOT_INS,
		// 	// PAPI_L1_DCM,
		// 	// PAPI_L1_DCA,
		// 	// PAPI_L2_DCM,
		// 	// PAPI_L2_DCA,
		// 	//PAPI_LST_INS,
		// 	//PAPI_flips
		// };

		// PAPI_library_init(PAPI_VER_CURRENT);
		// int w = PAPI_start_counters(PAPI_events, 2);

		// matrix multiplication
		for(int i = 0; i < d; i++)
		{
			for(int j = 0; j < d; j++)
			{
				for(int k = 0; k < d; k++)
				{
					result[i][j] += matA[i][k] * matB[k][j];
				}
			}
		}

		// for execution time:
		execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

		// PAPI_read_counters(counters, 2);

		// printf("Total cycles: %lld\nTotal instructions: %lld\n", counters[0], counters[1]);

		// printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);

		// printf("%lld L2 cache misses (%.3lf%% misses)\n", counters[2],(double)counters[2] / (double)counters[3]);

		// printf("Total load store instructions: %lld\nTotal floating point instructions: %lld\n", counters[0], counters[1]);

		// for execution time:
		printf("Real_time:\t%f seconds\nProc_time:\t%f seconds\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
		PAPI_shutdown();
	}
	else if(m == 'c') // clock_gettime
	{
		uint64_t execTime;
		struct timespec tick, tock;

		//clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tick);
		clock_gettime(CLOCK_REALTIME, &tick);

		// matrix multiplication
		for(int i = 0; i < d; i++)
		{
			for(int j = 0; j < d; j++)
			{
				for(int k = 0; k < d; k++)
				{
					result[i][j] += matA[i][k] * matB[k][j];
				}
			}
		}

		// clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tock);
		clock_gettime(CLOCK_REALTIME, &tock);

		execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec-tick.tv_nsec;
		printf("elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int)execTime);
	}

	// free the memory used for the matrices
	for (int i = 0; i < d; i++)
		free(matA[i]);
	free(matA);

	for (int i = 0; i < d; i++)
		free(matB[i]);
	free(matB);

	for (int i = 0; i < d; i++)
		free(result[i]);
	free(result);

	return 0;
}