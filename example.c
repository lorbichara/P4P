#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
//#include <papi.h>

#define MAX_STRING_LEN 80

int main( int argc, char *argv[] )
{
	char S[MAX_STRING_LEN];
	int l, i;
	  
	S[0] = 'a';
	S[1] = 'b';
	S[2] = 'c';
	S[3] = 'd';
	S[4] = 'e';
	S[5] = 'g';
	S[6] = '0';
	S[7] = 0;

	l = strlen(S);

	printf("S:\t%s\n",S);
	printf("length:\t%d\n",l);

	/* print characters in S */

	printf("forward\n");
	for (i = 0; i < l; ++i) 
		printf("A[%d] = %c\n",i,S[i]);
	// int i, j, k;

	// long long counters[3];
	// int PAPI_events[] = {
	// 		PAPI_TOT_CYC,
	// 		PAPI_L2_DCM,
	// 		PAPI_L2_DCA };

	// PAPI_library_init(PAPI_VER_CURRENT);
	// i = PAPI_start_counters( PAPI_events, 3 );

	//  your code here 

	// PAPI_read_counters( counters, 3 );

	// printf("%lld L2 cache misses (%.3lf%% misses) in %lld cycles\n", 
	// 	counters[1],(double)counters[1] / (double)counters[2],
	// 	counters[0] );

	return 0;
}