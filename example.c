#include <papi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main( int argc, char *argv[] )
{
	int i, j, k;

	long long counters[3];
	int PAPI_events[] = {
			PAPI_TOT_CYC,
			PAPI_L2_DCM,
			PAPI_L2_DCA };

	PAPI_library_init(PAPI_VER_CURRENT);
	i = PAPI_start_counters( PAPI_events, 3 );

	// your code here 
	int l, sum;
	l = 10;

	sum = l + 100;
	printf("hola\n");
	PAPI_read_counters( counters, 3 );

	printf("%lld L2 cache misses (%.3lf%% misses) in %lld cycles\n", 
		counters[1],(double)counters[1] / (double)counters[2],
		counters[0] );

	return 0;
}
