#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <math.h>
#include <iomanip>
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

//routine that reads a graph in DIMACS format from a file and constructs a Compressed-Sparse-Row (CSR) representation of that graph in memory..
void DIMACStoCSR(string fileName)
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

	f.close();

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

	for(int i = 0; i < rp.size(); i++)
	{
		cout << rp[i] << " "; 
	}
	cout << endl;

	for(int i = 0; i < ci.size(); i++)
	{
		cout << ci[i] << " "; 
	}
	cout << endl;

	for(int i = 0; i < ai.size(); i++)
	{
		cout << ai[i] << " "; 
	}

	cout << endl;
}

//routine that takes a graph in CSR representation in memory and prints it out to a file in DIMACS format. 
void CSRtoDIMACS(vector<int> rp, vector<int> ci, vector<int>ai)
{
	ofstream f;
	f.open("CSRtoDIMACS.dimacs");

	vector<int> source;
	int count = 1;
	for(int i = 1; i < rp.size(); i++)
	{
		//cout << rp[i] << " ";
		int subtraction = rp[i] - rp[i-1];
		while(subtraction != 0)
		{
			source.push_back(count);
			subtraction--;
		}
		count++;
	}

	int nEdges = ci.size();

	vector<int> allNodes = source;
	allNodes.insert(allNodes.end(), ai.begin(), ai.end());
	sort(allNodes.begin(), allNodes.end());
	int nNodes = std::unique(allNodes.begin(), allNodes.end()) - allNodes.begin();

	f << "p sp " << nNodes << " " << nEdges << endl;

	for(int i = 0; i < source.size(); i++)
	{
		if(i+1 != source.size())
			f << "a " << source[i] << " " << ci[i] << " " << ai[i] << endl;
		else
			f << "a " << source[i] << " " << ci[i] << " " << ai[i];
	}

	f.close();
}

void CSRtoFile(vector<int> rp, vector<int> ci, vector<int>ai)
{
	ofstream f;
	f.open("nodeNumbersNodeLabels.dimacs");

	f.close();
}

void pageRank(vector<int> rp, vector<int> ci, vector<int>ai)
{
	map<int, vector<int>> salientes;

	salientes.insert (pair<int,vector<int> >(2, {3}));
	salientes.insert (pair<int,vector<int> >(3, {2}));
	salientes.insert (pair<int,vector<int> >(4, {1, 2}));
	salientes.insert (pair<int,vector<int> >(5, {2, 4, 6}));
	salientes.insert (pair<int,vector<int> >(6, {2, 5}));
	salientes.insert (pair<int,vector<int> >(7, {2, 5}));
	salientes.insert (pair<int,vector<int> >(8, {2, 5}));
	salientes.insert (pair<int,vector<int> >(9, {2, 5}));
	salientes.insert (pair<int,vector<int> >(10, {5}));
	salientes.insert (pair<int,vector<int> >(11, {5}));

	map<int, vector<int>> entrantes;
	entrantes.insert (pair<int,vector<int> >(1, {4}));
	entrantes.insert (pair<int,vector<int> >(2, {3, 4, 5, 6, 7, 8, 9}));
	entrantes.insert (pair<int,vector<int> >(3, {2}));
	entrantes.insert (pair<int,vector<int> >(4, {5}));
	entrantes.insert (pair<int,vector<int> >(5, {6, 7, 8, 9, 10, 11}));
	entrantes.insert (pair<int,vector<int> >(6, {5}));

	map<int, double> ranks;
	int n = 11;
	double d = 0.85;
	double desiredError = 0.0001;
	double err = 10000;
	double oldRank = 0;
	double newRank = 0;
	vector<double> errors;

	for(int i = 0; i <= n; i++)
	{
		errors.push_back(10000.0);
	}

	errors[0] = 0.0;

	for(int i = 1; i <= n; i++)
	{
		ranks.insert (pair<int,double>(i, 1.0/n));
	}

	bool e = false;
	while(!e)
	{
		for (auto &i : ranks)
		{
			//para el 2 por ejemplo
			//int j = myMap.find(i.first)->second.size(); //esto devuelve el # de links entrantes a 2 para saber cuantas veces sumar

			double sum = 0;

			if (entrantes.find(i.first) == entrantes.end())
			{
				//cout << i.first << " ";
				// not found, meaning that this node doesn't have incoming links
				sum = i.second/n;

				oldRank = i.second;
				newRank = ((1-d)/n) + sum * d;
				errors[i.first] = abs(newRank - oldRank);
				i.second = newRank;
			}
			else
			{
				for(int k : entrantes.find(i.first)->second) //esto devuelve cada valor de los vectores. k es 1 numerito
				{
					sum += (ranks.find(k)->second)/salientes.find(k)->second.size();
				}

				oldRank = i.second;
				newRank = ((1-d)/n) + sum * d;
				errors[i.first] = abs(newRank - oldRank);
				i.second = newRank;
			}
	 	}

		if(all_of(errors.begin(), errors.end(), [desiredError](double i){return i < desiredError;}) )
			e = true;
		else
			e = false;
	}

cout << endl;
	for (const auto &p : ranks) {
	   // std::cout << "m[" << p.first << "] = " << p.second << '\n';
		cout << p.second << endl;
	}
}

int main()
{
	// string fileName;
	// cout << "Introduce the name of the file (with extension) that contains the graph in DIMACS format: ";
	// cin >> fileName;
	
	//DIMACStoCSR(fileName);
	// vector<int> rp = {0, 0, 1, 2, 4, 7, 9, 11, 13, 15, 16, 17}; 
	// vector<int> rp = {0, 0, 2, 3, 4}; 
	// vector<int> ci = {0, 1, 2, 1}; 
	// vector<int> ai = {5, 8, 3, 6}; 
	// vector<int> ci = {2, 3, 3, 4, 2, 5, 3, 6, 4, 6}; 
	// vector<int> ci = {3, 2, 1, 2, 2, 4, 6, 2, 5, 2, 5, 2, 5, 2, 5, 5, 5};
	// vector<int> ai = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
	// vector<int> ai = {16, 13, 10, 12, 4, 15, 9, 20, 7, 4}; 
	//CSRtoDIMACS(rp, ci, ai);
	pageRank(rp, ci, ai);
}