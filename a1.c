#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int d;
	printf("Matrix dimension? ");
	scanf("%d", &d);

	double matA[d][d];
	double matB[d][d];

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

	double result[d][d]; // matrix that will store the result of the mult
	
	// !!! INITIALIZE COUNTERS !!!
	// Total Cycles
	
	long long counters[2];

	int PAPI_events[] = {
		PAPI_TOT_CYC,
		PAPI_TOT_INS
	};

	PAPI_library_init(PAPI_VER_CURRENT);
	int i = PAPI_start_counters(PAPI_events, 2);

	// matrix multiplication
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			result[i][j] = 0;
			for(int k = 0; k < d; k++)
			{
				result[i][j] += matA[i][k] * matB[k][j];
			}
		}
	}

	PAPI_read_counters(counters, 2);

	printf("Total cycles: %f\nTotal instructions: %f\n", counters[0], counters[1]);

	// !!! STOP COUNTERS !!!

	// print result matrix
	/*for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			printf("%lf \t", result[i][j]);
		}

		printf("\n");
	}*/

	printf("Number of HW counters: %d\n", PAPI_num_counters());

	return 0;
}
