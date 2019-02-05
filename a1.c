#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	int d;
	printf("Matrix dimension? ");
	scanf("%d", &d);

	// initialize matrix A with random doubles
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			matA[i][j] = (double)rand()/RAND_MAX*2.0-1.0;
		}
	}

	// initialize matrix B with random doubles
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			matB[i][j] = (double)rand()/RAND_MAX*2.0-1.0;
		}
	}

	double result[d][d]; // matrix that will store the result of the mult
	
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

	// print result matrix
	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			printf("%lf \t", result[i][j]);
		}

		printf("\n");
	}

	printf("Number of HW counters: %d", PAPI_num_counters());

	return 0;
}