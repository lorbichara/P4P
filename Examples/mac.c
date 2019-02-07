#include <stdio.h>
#include <stdlib.h>

int main() {
	int d;
	printf("Matrix dimension? ");
	scanf("%d", &d);

	// //create matrices using malloc so they're big enough
	// double ** matA;
	// double ** matB;
	// double ** result;

	// matA = (double**)malloc(sizeof(double*)*d);
	// for(int i = 0; i < d; i++)
	// 	matA[i] = (double*)malloc(sizeof(double)*d);

	// matB = (double**)malloc(sizeof(double*)*d);
	// for(int i = 0; i < d; i++)
	// 	matB[i] = (double*)malloc(sizeof(double)*d);

	// result = (double**)malloc(sizeof(double*)*d);
	// for(int i = 0; i < d; i++)
	// 	result[i] = (double*)malloc(sizeof(double)*d);

	// // initialize matrix A with random doubles
	// for(int i = 0; i < d; i++)
	// {
	// 	for(int j = 0; j < d; j++)
	// 	{
	// 		double range = (100-1);
	// 		double div = RAND_MAX / range;
	// 		matA[i][j] = 1 + (rand() / div);
	// 	}
	// }

	// // initialize matrix B with random doubles
	// for(int i = 0; i < d; i++)
	// {
	// 	for(int j = 0; j < d; j++)
	// 	{
	// 		double range = (100-1);
	// 		double div = RAND_MAX / range;
	// 		matB[i][j] = 1 + (rand() / div);
	// 	}
	// }

	double matA[2][2] =
	{
		{1, 2},
		{3, 4}
	};

	double matB[2][2] =
	{
		{1, 2},
		{3, 4}
	};

	// double result[2][2] =
	// {
	// 	{0, 0},
	// 	{0, 0}
	// };

	double result[2][2];

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

	//print result matrix
	for(int i = 0; i < d; i++)
	{
	 	for(int j = 0; j < d; j++)
	 	{
	 		printf("%lf \t", result[i][j]);
	 	}

	 	printf("\n");
	 }

	// // free the memory used for the matrices
	// for (int i = 0; i < d; i++)
 //        free(matA[i]);
 //    free(matA);

 //    for (int i = 0; i < d; i++)
 //        free(matB[i]);
 //    free(matB);

 //    for (int i = 0; i < d; i++)
 //        free(result[i]);
 //    free(result);

	return 0;
}