#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */

using namespace std;

#define INFINITY 1073741823
vector<int> source;
vector<int> dest;
vector<int> weight;
int nNodes, nArcs;

void readGraph(string fileName)
{
	char comment;
	int d, w;
	string line, s, problem;
	ifstream f;
	f.open(fileName);

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
		else if(line[0] != 'p' && line[0] != 'c')
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

void bellmanFordSequential(string graphName)
{
	uint64_t execTime;
	struct timespec tick, tock;
	
	//set source node depending on graph used
	int sourceNode;
	if(graphName == "rmat15.dimacs" || graphName == "rmat23.dimacs")
		sourceNode = 1;
	else if(graphName == "road-NY.dimacs")
		sourceNode = 140961;
	else if(graphName == "road-FLA.dimacs")
		sourceNode = 316607;

	//initialize distances from source to all nodes
	//initial distance is infinity except for the source node itself
	vector<int> result(nNodes+1, INFINITY);
	result[sourceNode] = 0;

	clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
	//bellman ford
	for(int i = 1; i <= nNodes-1; i++)
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

int main(int argc, char *argv[])
{
	readGraph(argv[1]);
	cout << "ya leyó el graph" << endl;
	bellmanFordSequential(argv[1]);
}