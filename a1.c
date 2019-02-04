#include <papi.h>
#include <stdio.h>

int main() {
	double mat[2000][2000];
	int d;

	printf("Matrix dimension? ");
	scanf("%d", d);

	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			mat[i][j] = rand();
		}
	}

	for(int i = 0; i < d; i++)
	{
		for(int j = 0; j < d; j++)
		{
			printf("%lf \t", mat[i][j]);
		}

		printf("\n");
	}






	return 0;
}