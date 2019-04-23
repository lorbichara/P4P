#include <iostream>
#include <fstream>
#include <boost/array.hpp>
#include <boost/graph/edge_list.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>
#include <stdint.h>  /* for uint64  */
#include <time.h>    /* for clock_gettime */

#define INF 1073741823
std::vector<int> src;
std::vector<int> dest;
std::vector<int> weight;
int nNodes, nArcs;

void readGraph(std::string fileName)
{
	char comment;
	int d, w;
	std::string line, s, problem;
	std::ifstream f;
	f.open(fileName.c_str());

	if(!f)
	{
		std::cout << "File wasn't found." << std::endl;
		exit(1);
	}

	while(getline(f, line))
	{
		std::istringstream stream(line);
		if(line[0] == 'p')
		{
			stream >> comment >> problem >> nNodes >> nArcs;
		}
		else if(line[0] != 'p' && line[0] != 'c')
		{
			stream >> comment >> s >> d >> w;
			src.push_back(atoi(s.c_str()));
			dest.push_back(d);
			weight.push_back(w);
		}
	}

	std::cout << nNodes << " " << nArcs << std::endl;

	f.close();
}

int main(int argc, char *argv[])
{
	readGraph(argv[1]);

	uint64_t execTime;
	struct timespec tick, tock;

	using namespace boost;

	//setup router network
	int n_vertices = nNodes;
	int n_edges = nArcs;
	typedef std::pair<int, int> Edge;

	std::vector<Edge> edges;
	for(int i = 0; i < n_edges; i++)
	{
		edges.push_back(Edge(src[i], dest[i]));
	}

	//Specify graph type and declare a graph object
	typedef edge_list<std::vector<Edge>::iterator> Graph;
	Graph g(edges.begin(), edges.end());

	//Assign edge weights
	std::vector<int> delay;
	for(int i = 0; i < n_edges; i++)
	{
		delay.push_back(weight[i]);
	}

	//Create vertex property storage
	std::vector<int> parent;
	for(int i = 0; i < n_vertices; ++i)
		parent.push_back(i);

	std::vector<int> distance;
	for(int i = 0; i < n_vertices+1; i++)
	{
		distance.push_back(INF);
	}

	//Specify A as source vertex
	int sourceNode;
	std::size_t found;

	std::string name = argv[1];

	if(name.find("rmat15.dimacs") != std::string::npos || name.find("rmat23.dimacs") != std::string::npos)
		sourceNode = 1;
	else if(name.find("road-NY.dimacs") != std::string::npos)
		sourceNode = 140961;
	else if(name.find("road-FLA.dimacs") != std::string::npos)
		sourceNode = 316607;
	distance[sourceNode] = 0;

	clock_gettime(CLOCK_MONOTONIC_RAW, &tick);
	
	bool r = bellman_ford_shortest_paths(g, int(n_vertices),
		weight_map(make_iterator_property_map(delay.begin(), get(edge_index, g), delay[0])).
		distance_map(&distance[0]).
		predecessor_map(&parent[0]));

	clock_gettime(CLOCK_MONOTONIC_RAW, &tock);
	execTime = 1000000000 * (tock.tv_sec - tick.tv_sec) + tock.tv_nsec - tick.tv_nsec;
	printf("Elapsed process CPU time = %llu nanoseconds\n", (long long unsigned int) execTime);

	//print result to output file
	std::ofstream output;
	output.open("output.txt");
	if(r)
	{
		for(int i = 1; i <= n_vertices; i++)
		{
			output << i << " " << distance[i] << std::endl;
		}
	}
	output.close();
}