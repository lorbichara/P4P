// Lorraine Bichara - lb34995
// CS 377P
// march 18, 2019
#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <time.h>
#include <immintrin.h>
#include "mkl.h"

int min(int x, int y) 
{ 
	return y ^ ((x ^ y) & -(x < y)); 
} 

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

//Naive
//function that takes 3 NxN matrices as input and performs matrix multiplication using ikj
void MMM(int matrixSize)
{
	float **a = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **b = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **c = Allocate2DArray_Offloat(matrixSize, matrixSize);

	srand(time(0));

	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			a[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			b[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	//PAPI measurements
	// long long counters[2];
	// int PAPI_events[] = {
	// 	PAPI_L1_DCM,
	// 	PAPI_L1_DCA
	// };
	// PAPI_library_init(PAPI_VER_CURRENT);
	// int w = PAPI_start_counters(PAPI_events, 2);

	float real_time, proc_time, mflops;
	long long flpins;
	int execTime;
	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	for(int j = 0; j < matrixSize; j++)
	{
		for(int i = 0; i < matrixSize; i++)
		{
			for(int k = 0; k < matrixSize; k++)
			{
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}

	//PAPI measurements
	// PAPI_read_counters(counters, 2);
	// printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);
	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);
	printf("Mflops: %f\n", mflops);
	PAPI_shutdown();
	
	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");

	//Free memory
	Free2DArray((void**)a);
	Free2DArray((void**)b);
	Free2DArray((void**)c);
}

//Register blocking
void MMMRegisterBlocking(int NB)
{
	//matrix size NB = N
	int MU = 4; //multiple of 4
	int NU = 1;

	//create matrices of size NB
	float **A = Allocate2DArray_Offloat(NB, NB);
	float **B = Allocate2DArray_Offloat(NB, NB);
	float **C = Allocate2DArray_Offloat(NB, NB);

	srand(time(0));

	for(int i = 0; i < NB; i++)
	{
		for(int j = 0; j < NB; j++)
		{
			A[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			B[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	//PAPI measurements
	// long long counters[2];
	// int PAPI_events[] = {
	// 	PAPI_L1_DCM,
	// 	PAPI_L1_DCA
	// };
	// PAPI_library_init(PAPI_VER_CURRENT);
	// int w = PAPI_start_counters(PAPI_events, 2);

	float real_time, proc_time, mflops;
	long long flpins;
	int execTime;
	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	//mini-kernel
	for(int j = 0; j < NB; j+=NU)
	{
		for(int i = 0; i < NB; i+=MU)
		{
			//load C[i..i+MU-1, j..j+NU-1] into registers
			//j stays fixed because NU = 1
			register float c1 = C[i][j];
			register float c2 = C[i+1][j];
			register float c3 = C[i+2][j];
			register float c4 = C[i+3][j];

			for(int k = 0; k < NB; k++)
			{
				//micro-kernel
				//load A[i..i+MU-1,k] into registers
				register float a1 = A[i][k];
				register float a2 = A[i+1][k];
				register float a3 = A[i+2][k];
				register float a4 = A[i+3][k];

				//load B[k,j..j+NU-1] into registers
				register float b1 = B[k][j]; //NU = 1 so we only load B[k][j]

				//multiply A's and B's and add to C's
				//store C[i..i+MU-1, j..j+NU-1]
				//C[i][j] += A[i][k] * B[k][j];
				c1 += a1 * b1;
				c2 += a2 * b1;
				c3 += a3 * b1;
				c4 += a4 * b1;

				C[i][j] = c1;
				C[i+1][j] = c2;
				C[i+2][j] = c3;
				C[i+3][j] = c4;
			}
		}
	}

	//PAPI measurements
	// PAPI_read_counters(counters, 2);
	// printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);
	// PAPI_shutdown();

	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);
	printf("Mflops: %f\n", mflops);
	PAPI_shutdown();

	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");

	//Free memory
	Free2DArray((void**)A);
	Free2DArray((void**)B);
	Free2DArray((void**)C);
}

//Vectorized register blocking
void MMMVectorizedRegisterBlocking(int NB)
{
	int MU = 4;
	int NU = 1;

	//create matrices of size NB
	float **A = Allocate2DArray_Offloat(NB, NB);
	float **B = Allocate2DArray_Offloat(NB, NB);
	float **C = Allocate2DArray_Offloat(NB, NB);

	srand(time(0));

	for(int i = 0; i < NB; i++)
	{
		for(int j = 0; j < NB; j++)
		{
			A[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			B[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	//PAPI measurements
	// long long counters[2];
	// int PAPI_events[] = {
	// 	PAPI_L1_DCM,
	// 	PAPI_L1_DCA
	// };
	// PAPI_library_init(PAPI_VER_CURRENT);
	// int w = PAPI_start_counters(PAPI_events, 2);

	float real_time, proc_time, mflops;
	long long flpins;
	int execTime;
	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	//mini-kernel
	for(int j = 0; j < NB; j+=NU)
	{
		for(int i = 0; i < NB; i+=MU)
		{
			//load C[i..i+MU-1, j..j+NU-1] into registers
			float helper[4] = {C[i][j], C[i+1][j], C[i+2][j], C[i+3][j]};
			__m128 c = _mm_load_ps(helper);
			//__m128 c = _mm_set_ps(C[i][j], C[i+1][j], C[i+2][j], C[i+3][j]);

			for(int k = 0; k < NB; k++)
			{
				//micro-kernel
				//load A[i..i+MU-1,k] into registers
				__m128 a = _mm_set_ps(A[i][k], A[i+1][k], A[i+2][k], A[i+3][k]);

				//load B[k,j..j+NU-1] into registers
				__m128 b = _mm_set_ps(B[k][j], B[k][j], B[k][j], B[k][j]);

				//multiply A's and B's and add to C's
				//store C[i..i+MU-1, j..j+NU-1]
				__m128 d = _mm_mul_ps(a, b);
				c = _mm_add_ps(c, d);
				float temp[4];
				_mm_store_ps(&temp, c);

				C[i][j] = temp[3];
				C[i+1][j] = temp[2];
				C[i+2][j] = temp[1];
				C[i+3][j] = temp[0];
			}
		}
	}

	//PAPI measurements
	// PAPI_read_counters(counters, 2);
	// printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);

	execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);
	printf("Mflops: %f\n", mflops*4*4);
	
	PAPI_shutdown();

	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");

	//Free memory
	Free2DArray((void**)A);
	Free2DArray((void**)B);
	Free2DArray((void**)C);
}

//Register-blocking and L1 cache-blocking
void MMMCacheRegisterBlocking(int N)
{
	int MU = 4;
	int NU = 1;
	int NB = 4;

	//create matrices of size NB
	float **A = Allocate2DArray_Offloat(N, N);
	float **B = Allocate2DArray_Offloat(N, N);
	float **C = Allocate2DArray_Offloat(N, N);

	srand(time(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			A[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			B[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	//PAPI measurements
	long long counters[2];
	int PAPI_events[] = {
		PAPI_L1_DCM,
		PAPI_L1_DCA
	};
	PAPI_library_init(PAPI_VER_CURRENT);
	int w = PAPI_start_counters(PAPI_events, 2);

	// float real_time, proc_time, mflops;
	// long long flpins;
	// int execTime;
	// execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	for(int bj = 0; bj < N; bj+=NB)
	{
		for(int bi = 0; bi < N; bi+=NB)
		{
			for(int bk = 0; bk < N; bk+=NB)
			{
				//mini-kernel
				for(int j = bj; j < min(bj + NB, N); j+=NU)
				{
					for(int i = bi; i < min(bi + NB, N); i+=MU)
					{
						//load C[i..i+MU-1, j..j+NU-1] into registers
						float helper[4] = {C[i][j], C[i+1][j], C[i+2][j], C[i+3][j]};
						__m128 c = _mm_load_ps(helper);
						//__m128 c = _mm_set_ps(C[i][j], C[i+1][j], C[i+2][j], C[i+3][j]);

						for(int k = bk; k < min(bk + NB, N); k++)
						{
							//micro-kernel
							//load A[i..i+MU-1,k] into registers
							__m128 a = _mm_set_ps(A[i][k], A[i+1][k], A[i+2][k], A[i+3][k]);

							//load B[k,j..j+NU-1] into registers
							__m128 b = _mm_set_ps(B[k][j], B[k][j], B[k][j], B[k][j]);

							//multiply A's and B's and add to C's
							//store C[i..i+MU-1, j..j+NU-1]
							__m128 d = _mm_mul_ps(a, b);
							c = _mm_add_ps(c, d);
							float temp[4];
							_mm_store_ps(&temp, c);

							C[i][j] = temp[3];
							C[i+1][j] = temp[2];
							C[i+2][j] = temp[1];
							C[i+3][j] = temp[0];
						}
					}
				}
			}
		}
	}

	//PAPI measurements
	PAPI_read_counters(counters, 2);
	printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);

	// execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);
	// printf("Mflops: %f\n", mflops*4*4);
	
	PAPI_shutdown();

	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");

	//Free memory
	Free2DArray((void**)A);
	Free2DArray((void**)B);
	Free2DArray((void**)C);
}

//Implement Copying
void MMMCopying(int N)
{
	int MU = 4;
	int NU = 1;
	int NB = 8;

	//create matrices of size NB
	float **A = Allocate2DArray_Offloat(N, N);
	float **B = Allocate2DArray_Offloat(N, N);
	float **C = Allocate2DArray_Offloat(N, N);

	srand(time(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			A[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			B[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	//PAPI measurements
	// long long counters[2];
	// int PAPI_events[] = {
	// 	PAPI_L1_DCM,
	// 	PAPI_L1_DCA
	// };
	// PAPI_library_init(PAPI_VER_CURRENT);
	// int w = PAPI_start_counters(PAPI_events, 2);

	// float real_time, proc_time, mflops;
	// long long flpins;
	// int execTime;
	// execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);

	//Copy A here
	float copyA[N*N];
	for(int t = 0; t < N; t++)
	{
		for(int u = 0; u < N; u++)
		{
			memcpy(&copyA[t*N+u], &A[t][u], 4);
		}
	}
	

	for(int t = 0; t < N*N; t++)
	{
		printf("7.2%f ", copyA[t]);
	}

	// for(int bj = 0; bj < N; bj+=NB)
	// {
	// 	//Copy a row/column of B here
	// 	float copyB[N];
	// 	for(int l = 0; l < N; l++)
	// 	{
	// 		memcpy(&copyB[l], &B[l][bj], 4);
	// 	}
		
	// 	for(int bi = 0; bi < N; bi+=NB)
	// 	{
	// 		//Copy an element of C here
	// 		float copyC[NB];
	// 		for(int l = 0; l < N; l++)
	// 		{
	// 			memcpy(&copyC[l], &C[bi][bj], 4);
	// 		}
	// 		for(int bk = 0; bk < N; bk+=NB)
	// 		{
	// 			//mini-kernel
	// 			for(int j = bj; j < min(bj + NB, N); j+=NU)
	// 			{
	// 				for(int i = bi; i < min(bi + NB, N); i+=MU)
	// 				{
	// 					//load C[i..i+MU-1, j..j+NU-1] into registers
	// 					float helper[4] = {copyC[0], copyC[1], copyC[2], copyC[3]};
	// 					__m128 c = _mm_load_ps(helper);
	// 					//__m128 c = _mm_set_ps(C[i][j], C[i+1][j], C[i+2][j], C[i+3][j]);

	// 					for(int k = bk; k < min(bk + NB, N); k++)
	// 					{
	// 						//micro-kernel
	// 						//load A[i..i+MU-1,k] into registers
	// 						__m128 a = _mm_set_ps(copyA[i], copyA[i+1], copyA[i+2], copyA[i+3]);

	// 						//load B[k,j..j+NU-1] into registers
	// 						__m128 b = _mm_load_ps(copyB);

	// 						//multiply A's and B's and add to C's
	// 						//store C[i..i+MU-1, j..j+NU-1]
	// 						__m128 d = _mm_mul_ps(a, b);
	// 						c = _mm_add_ps(c, d);
	// 						float temp[4];
	// 						_mm_store_ps(&temp, c);

	// 						copyC[0]= temp[3];
	// 						copyC[1] = temp[2];
	// 						copyC[2] = temp[1];
	// 						copyC[3] = temp[0];

	// 						memcpy(&C[i][j], &copyC[0], 4);
	// 						memcpy(&C[i+1][j], &copyC[1], 4);
	// 						memcpy(&C[i+2][j], &copyC[2], 4);
	// 						memcpy(&C[i+3][j], &copyC[3], 4);
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N; j++)
		{
			printf("%7.2f\t", A[i][j]);
		}
		printf("\n");
	}

	// for(int i = 0; i < N; i++)
	// {
	// 	for(int j = 0; j < N; j++)
	// 	{
	// 		printf("%7.2f\t", B[i][j]);
	// 	}
	// 	printf("\n");
	// }

	// for(int i = 0; i < N; i++)
	// {
	// 	for(int j = 0; j < N; j++)
	// 	{
	// 		printf("%7.2f\t", C[i][j]);
	// 	}
	// 	printf("\n");
	// }

	//PAPI measurements
	// PAPI_read_counters(counters, 2);
	// printf("%lld L1 cache misses (%.3lf%% misses)\n", counters[0],(double)counters[0] / (double)counters[1]);

	// execTime=PAPI_flops(&real_time, &proc_time, &flpins, &mflops);
	// printf("Mflops: %f\n", mflops*4*4);
	
	// PAPI_shutdown();

	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");

	//Free memory
	Free2DArray((void**)A);
	Free2DArray((void**)B);
	Free2DArray((void**)C);
}

void MMMMKL(int matrixSize)
{
	float **A = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **B = Allocate2DArray_Offloat(matrixSize, matrixSize);
	float **C = Allocate2DArray_Offloat(matrixSize, matrixSize);

	double alpha = 1.0;
	double beta = 0.0;

	srand(time(0));

	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			A[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
			B[i][j] = (float)rand()/(float)(RAND_MAX/20.000);
		}
	}

	//cleaning cache
	const size_t bigger_than_cachesize = 10 * 1024 * 1024;
	char *z = (char *)malloc(bigger_than_cachesize);
	for(int i = 0; i < bigger_than_cachesize; i++)
	{
		z[i] = 0;
	}

	//CPUID to flush pipeline and serialize instructions
	int p, q;
	__asm__("cpuid"
			:"=a"(q)
			:"0"(p)
			:"%ebx","%ecx","%edx");

	cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, matrixSize, matrixSize, matrixSize, alpha, A, matrixSize, B, matrixSize, beta, C, matrixSize);
	
	//CPUID to flush pipeline and serialize instructions
	int x, y;
	__asm__("cpuid"
			:"=a"(y)
			:"0"(x)
			:"%ebx","%ecx","%edx");


	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			printf("%7.2f\t", A[i][j]);
		}
		printf("\n");
	}

	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			printf("%7.2f\t", B[i][j]);
		}
		printf("\n");
	}

	for(int i = 0; i < matrixSize; i++)
	{
		for(int j = 0; j < matrixSize; j++)
		{
			printf("%7.2f\t", C[i][j]);
		}
		printf("\n");
	}

	//Free memory
	Free2DArray((void**)A);
	Free2DArray((void**)B);
	Free2DArray((void**)C);
}

int main(int argc, char *argv[])
{
	int matrixSize = atoi(argv[1]);

	printf("Type in a letter based on the matrix multiplication version you want to use:\n");
	printf("a - Naive\nb - Register Blocking\nc - Vectorized Register Blocking\nd - Cache Register Blocking\ne - Copying\nf - MLK\n");

	char question;
	scanf(" %c", &question);

	switch(question)
	{
		case 'a':
			MMM(matrixSize);
			break;
		case 'b':
			MMMRegisterBlocking(matrixSize);
			break;
		case 'c':
			MMMVectorizedRegisterBlocking(matrixSize);
			break;
		case 'd':
			MMMCacheRegisterBlocking(matrixSize);
			break;
		case 'e':
			MMMCopying(matrixSize);
			break;
		case 'f':
			MMMMKL(matrixSize);
			break;
	}
}
