//Lorraine Bichara lb34995
//Programming for Performance
//Assignment 2: Graph algorithms

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <math.h>
#include <iomanip>
using namespace std;

//struct that'll help me convert from COO to CSR
struct cooRep {
	int source;
	int destination;
	int value;

	bool operator()(cooRep i, cooRep j)
	{ 
		if(i.source != j.source)
			return (i.source < j.source);
		else
			return (i.destination < j.destination);
	}
};

//struct that holds the CSR representation
struct CSRRep {
	vector<int> rp;
	vector<int> ci;
	vector<int> ai;
};

//Routine that reads a graph in DIMACS format from a file and constructs a Compressed-Sparse-Row (CSR) representation of that graph in memory.
//Returns a CSR struct that holds 3 vectors.
struct CSRRep DIMACStoCSR(string fileName)
{
	string line;
	ifstream f;
	f.open(fileName);

	//if the file wasn't found
	if(!f) {
		cout << "File wasn't found." << endl;
		exit(1);
	}

	//CONSTRUCT COORDINATE REPRESENTATION (COO) OF THE GRAPH FROM THE DIMACS FILE
	vector<cooRep> cooVector;
	char a;
	string b;
	int c, d;
	int nRows; //number of rows = number of nodes

	//save file values into a vector of structs
	while(f >> a >> b >> c >> d)
	{
		if(a == 'p')
		{
			nRows = (int)c;
		}
		else if(a != 'c' && a != 'p') //to ignore comments and the problem line
		{
			cooRep coo = {atoi(b.c_str()), (int)c, (int)d};
			cooVector.push_back(coo);
		}
	}

	f.close();

	//sort edges in COO (vector of structs) by the source node ID
	sort(cooVector.begin(), cooVector.end(), cooRep());

	//construct CSR representation from the information in the COO
	vector<int> rp;
	vector<int> ci;
	vector<int> ai;

	//breakdown vector of structs into individual vectors
	for(int i = 0; i < cooVector.size(); i++)
	{
		ci.push_back(cooVector[i].destination);
		ai.push_back(cooVector[i].value); 
	}

	map<int, int> occurenceMap;
	map<int, int>::iterator it;

	for(int i = 0; i < cooVector.size(); i++)
	{
		if(occurenceMap.find(cooVector[i].source) == occurenceMap.end())
			occurenceMap[cooVector[i].source] = 1;
		else
			occurenceMap[cooVector[i].source]++;
	}

	it = occurenceMap.begin();
	int NNZ = 0;

	for(auto it: occurenceMap)
	{
		rp.push_back(NNZ);
		NNZ += it.second;
	}
	
	for(int i = 0; i <= nRows-occurenceMap.size(); i++)
	{
		rp.push_back(NNZ);
	}

	CSRRep csr;
	csr.rp = rp;
	csr.ci = ci;
	csr.ai = ai;

	return csr;
}

//Routine that takes a graph in CSR representation in memory and prints it out to a file in DIMACS format.
void CSRtoDIMACS(struct CSRRep csr)
{
	ofstream f;
	f.open("CSRtoDIMACS.dimacs");

	//make a vector that includes source nodes using the rp vector.
	vector<int> source;
	int count = 0;
	if(csr.rp[0] == 0)
		count = 2;
	else
		count = 1;

	for(int i = 1; i < csr.rp.size(); i++)
	{
		int subtraction = csr.rp[i] - csr.rp[i-1];
		while(subtraction != 0)
		{
			source.push_back(count);
			subtraction--;
		}
		count++;
	}

	int nEdges = csr.ci.size(); //number of edges

	//get number of nodes
	vector<int> allNodes = source;
	allNodes.insert(allNodes.end(), csr.ai.begin(), csr.ai.end());
	sort(allNodes.begin(), allNodes.end());
	int nNodes = std::unique(allNodes.begin(), allNodes.end()) - allNodes.begin();

	//print to file
	f << "p sp " << nNodes << " " << nEdges << endl;

	for(int i = 0; i < source.size(); i++)
	{
		if(i+1 != source.size())
			f << "a " << source[i] << " " << csr.ci[i] << " " << csr.ai[i] << endl;
		else
			f << "a " << source[i] << " " << csr.ci[i] << " " << csr.ai[i];
	}

	f.close();
}

//Routine that takes a graph in CSR representation in memory, and prints node numbers and node labels, one per line, to a file.
//Node number: distinguishes it from other nodes and to find its edges in the CSR format representation of the graph.
//Node label: problem-dependent quantities associated with a particular graph problem you want to solve, what need to be computed.
void CSRtoFile(map<int, double> ranks)
{
	ofstream f;
	f.open("nodeNumbersNodeLabels.dimacs");

	for (auto myMap : ranks)
	{
		f << myMap.first << " " << myMap.second << endl;
	}

	f.close();
}

//Page Rank Algorithm
//Takes 3 vectors as arguments that represent the CSR format.
//Doesn't have a return value, but prints the result of the page rank.
void pageRank(struct CSRRep csr)
{
	//get vector that stores source nodes
	vector<int> source;
	int count = 0;
	if(csr.rp[0] == 0)
		count = 2;
	else
		count = 1;

	for(int i = 1; i < csr.rp.size(); i++)
	{
		int subtraction = csr.rp[i] - csr.rp[i-1];
		while(subtraction != 0)
		{
			source.push_back(count);
			subtraction--;
		}
		count++;
	}

	//get total amount of nodes = n
	vector<int> allNodes = source;
	allNodes.insert(allNodes.end(), csr.ai.begin(), csr.ai.end());
	sort(allNodes.begin(), allNodes.end());
	int nNodes = std::unique(allNodes.begin(), allNodes.end()) - allNodes.begin();

	//map that holds each source and its outgoing links
	map<int, vector<int>> outgoing;
	for(int i = 0; i < source.size(); i++)
	{
		outgoing[source[i]].push_back(csr.ci[i]);
	}

	//map that holds each source and its incoming links
	map<int, vector<int>> incoming;
	for(int i = 0; i < csr.ci.size(); i++)
	{
		incoming[csr.ci[i]].push_back(source[i]);
	}

	double d = 0.85;
	double desiredError = pow(10, -4);
	double oldRank = 0;
	double newRank = 0;

	//use a vector to calculate errors for each page rank
	//errors[1] correspond to page rank for node 1, so errors[0] was set to 0 to ignore it
	//initialize it with 1000
	vector<double> errors;
	for(int i = 0; i <= nNodes; i++)
	{
		errors.push_back(10000.0);
	}
	errors[0] = 0.0;

	//map that'll hold the page rank for each node
	map<int, double> ranks;
	for(int i = 1; i <= nNodes; i++)
	{
		ranks.insert (pair<int,double>(i, 1.0/nNodes));
	}

	bool e = false;
	while(!e) //while the errors are still larger than the threshold of 10^-4
	{
		for (auto &i : ranks) //loop through the map that holds the rank for each node/page
		{
			double sum = 0;

			if (incoming.find(i.first) == incoming.end()) // not found in map, meaning that this node doesn't have incoming links
			{
				sum = i.second/nNodes; // new page rank = old page rank/n
			}
			else
			{
				for(int k : incoming.find(i.first)->second) //loop through each "incoming node" to the current node we're calculating
				{
					//for each incoming node, take its page rank and divide it by its number of outgoing links (found in outgoing map)
					sum += (ranks.find(k)->second)/outgoing.find(k)->second.size();
				}
			}

			//update page rank and errors
			oldRank = i.second;
			newRank = ((1-d)/nNodes) + sum * d;
			errors[i.first] = abs(newRank - oldRank);
			i.second = newRank;
	 	}

	 	//check if all errors for every page rank are smaller than the threshold of 10^-4
		if(all_of(errors.begin(), errors.end(), [desiredError](double i){return i < desiredError;}) )
			e = true;
		else
			e = false;
	}

	//scale page rank so the sum of all nodes is = 1 and print results
	double totalSum = 0;
	for (const auto &p : ranks)
	{
		totalSum += p.second;
	}

	for (auto &q : ranks)
	{
		q.second = q.second/totalSum;
		cout << "Node " << q.first << ": " << q.second << endl;
	}

	CSRtoFile(ranks); //Call routine that outputs node numbers and labels to a file.
}

int main(int argc, char *argv[])
{
	CSRRep csr = DIMACStoCSR(argv[1]);
	pageRank(csr);
}