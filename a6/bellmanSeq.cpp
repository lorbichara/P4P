#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

using namespace std;

vector<int> source;
vector<int> dest;
vector<int> weight;

void readGraph(string fileName)
{
	char comment;
	int d, w, nNodes, nArcs;
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

int main(int argc, char *argv[])
{
	readGraph(argv[1]);
}