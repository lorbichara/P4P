#include <papi.h>
#include <stdio.h>
#include <stdlib.h>
static void test_fail(char *file, int line, char *call, int retval);
int main() {
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

	int d;
	printf("Matrix dimension? ");
	scanf("%d", &d);

	//create matrices using malloc so they're big enough
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

	float real_time, proc_time, mflops;
	long long flpins;
	int retval;

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

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

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	// matrix multiplication
	for(int j = 0; j < d; j++)
	{
		for(int i = 0; i < d; i++)
		{
			for(int k = 0; k < d; k++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	// matrix multiplication
	for(int j = 0; j < d; j++)
	{
		for(int k = 0; k < d; k++)
		{
			for(int i = 0; i < d; i++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	// matrix multiplication
	for(int k = 0; k < d; k++)
	{
		for(int j = 0; j < d; j++)
		{
			for(int i = 0; i < d; i++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	// matrix multiplication
	for(int i = 0; i < d; i++)
	{
		for(int k = 0; k < d; k++)
		{
			for(int j = 0; j < d; j++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

	/* Setup PAPI library and begin collecting data from the counters */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	// matrix multiplication
	for(int k = 0; k < d; k++)
	{
		for(int i = 0; i < d; i++)
		{
			for(int j = 0; j < d; j++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	/* Collect the data into the variables passed in */
	retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops);

	printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",real_time, proc_time, flpins, mflops);
	PAPI_shutdown();

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
