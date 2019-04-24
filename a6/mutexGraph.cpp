//Thread Bellman Ford
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */
#include <pthread.h>
#include <stdlib.h>

using namespace std;

#define INFINITY 1073741823
#define MAX_THREADS 8
pthread_t handles[MAX_THREADS];
int shortNames[MAX_THREADS];

void *bellman_ford (void *);

vector<int> source;
vector<int> dest;
vector<int> weight;
vector<int> result;
int nNodes, nArcs;
int NUM_THREADS;

void readGraph(string fileName)
{
	char comment;
	int d, w;
	string line, s, problem;
	ifstream f;
	f.open(fileName.c_str());

	if(!f)
	{
		cout << "File wasn't found." << endl;
		exit(1);
	}

	while(getline(f, line))
	{
		istringstream stream(line);
		if(line[0] == 'p')
		{
			stream >> comment >> problem >> nNodes >> nArcs;
		}
		else if(line[0] == 'a')
		{
			stream >> comment >> s >> d >> w;
			source.push_back(atoi(s.c_str()));
			dest.push_back(d);
			weight.push_back(w);
		}
	}

	cout << nNodes << " " << nArcs << endl;

	f.close();
}

int main(int argc, char *argv[])
{
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	pthread_attr_t attr;
	pthread_attr_init (&attr);

	//read graph
	string graphName = argv[1];
	readGraph(graphName);

	//set source node
	int sourceNode;
	if(graphName == "rmat15.dimacs" || graphName == "rmat23.dimacs")
		sourceNode = 1;
	else if(graphName == "road-NY.dimacs")
		sourceNode = 140961;
	else if(graphName == "road-FLA.dimacs")
		sourceNode = 316607;

	result.assign(nNodes+1, INFINITY);
	result[sourceNode] = 0;

	//execute bellman_ford
	uint64_t execTime;
	struct timespec tick, tock;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tick);

	NUM_THREADS = atoi(argv[2]); //number of threads is an input

	//create threads
	for(int i = 0; i < NUM_THREADS; i++)
	{
		//printf("Creating thread %d\n", i);
		shortNames[i] = i;
		pthread_create(& handles[i], &attr, bellman_ford, & shortNames[i]);
	}

	//join with threads. No need to add contributions since they were directly added to the global variable
	for(int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(handles[i], NULL);
	}

	clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
	execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
	printf("Elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) execTime);

	//print result to output file
	ofstream output;
	output.open("output.txt");
	for(int i = 1; i <= nNodes; i++)
	{
		output << i << " " << result[i] << endl;
	}
	output.close();
}

void *bellman_ford(void *threadIdPtr)
{
	int myId = *(int *)threadIdPtr;

	for(int i = myId; i <= nNodes - 1; i+=NUM_THREADS)
	{
		for(int j = 0; j < nArcs; j++)
		{
			int u = source[j];
			int v = dest[j];
			int wt = weight[j];

			if((result[u] != INFINITY) && (result[u] + wt < result[v]))
				result[v] = result[u] + wt;
		}
		cout << i << " ";
	}
	cout << endl;
}