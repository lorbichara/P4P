#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
using namespace std;

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

void readGraph(string fileName)
{	
	string line;
	ifstream f;
	f.open(fileName);

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

	//sort edges in COO (vector of structs) by the source node ID
	sort(cooVector.begin(), cooVector.end(), cooRep());

	//construct CSR representation from the information in the COO
	vector<int> rp;
	vector<int> ci;
	vector<int> ai;

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
	int NNZ = 1;

	for(auto it: occurenceMap)
	{
		rp.push_back(NNZ);
		NNZ += it.second;
	}
	
	for(int i = 0; i <= nRows-occurenceMap.size(); i++)
	{
		rp.push_back(NNZ);
	}

	for(int i = 0; i < rp.size(); i++)
	{
		cout << rp[i] << " "; 
	}
	cout << endl;
}

int main()
{
	string fileName;
	cout << "Introduce the name of the file (with extension) that contains the graph in DIMACS format: ";
	cin >> fileName;
	
	readGraph(fileName);
}