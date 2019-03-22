// Lorraine Bichara - lb34995
// CS 377P
// march 18, 2019

#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <time.h>

//allocation routine to allocate storage
float **Allocate2DArray_Offloat(int x, int y)
{
	int TypeSize = sizeof(float);
	float **ppi = malloc(x * sizeof(float*));
	float *pool = malloc(x * y * TypeSize);
	unsigned char * curPtr = pool;
	int i;

	if(!ppi || !pool)
	{
		/*Quit if either allocation failed*/
		if(ppi)
			free(ppi);
		if(pool)
			free(pool);
		return NULL;
	}

	/*Create row vector*/
	for(i = 0; i < x; i++)
	{
		*(ppi + i) = curPtr;
		curPtr += y*TypeSize;
	}

	return ppi;
}

void Free2DArray(void ** Array)
{
	free(*Array);
	free(Array);
}

//function that takes 3 NxN matrices as input and performs matrix multiplication using ikj
void MMM() {
	int matrixSize;
	scanf("%d", &matrixSize);

	float **a = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **b = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **c = Allocate2DArray_Offloat(matrixSize, matrixSize);

	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			a[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			b[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	float real_time, proc_time, mflops;
	long long flpins;
	int execTime;
	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	long long counters[2];
	int PAPI_events[] = {
		PAPI_L1_DCM,
		PAPI_L1_DCA
	};

	PAPI_library_init(PAPI_VER_CURRENT);
	int w = PAPI_start_counters(PAPI_events, 2);

	for(int i = 0; i < matrixSize; i++)
	{
		for(int k = 0; k < matrixSize; k++)
		{
			for(int j = 0; j < matrixSize; j++)
			{
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	PAPI_read_counters(counters, 2);
	printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);
	//PAPI_shutdown();

	printf("Matrix size: %d\n", matrixSize);

	Free2DArray((void**)a);
	Free2DArray((void**)b);
	Free2DArray((void**)c);
}

int main() {
	MMM();
}