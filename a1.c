#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
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

	// !!! INITIALIZE COUNTERS !!!	
	long long counters[2];

	int PAPI_events[] = {
		PAPI_TOT_CYC,
 		PAPI_TOT_INS
	};

	PAPI_library_init(PAPI_VER_CURRENT);
	int i = PAPI_start_counters(PAPI_events, 2);

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

	PAPI_read_counters(counters, 2);

	printf("Total cycles: %lld\nTotal instructions: %lld\n", counters[0], counters[1]);

	// !!! STOP COUNTERS !!!

	//print result matrix
	/*for(int i = 0; i < d; i++)
	{
	 	for(int j = 0; j < d; j++)
	 	{
	 		printf("%lf \t", result[i][j]);
	 	}

	 	printf("\n");
	 }*/

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

    // flush pipeline
    // CPUID

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

	return 0;
}
