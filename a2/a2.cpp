#include <iostream>
using namespace std

void readGraph(string fileName)
{
	string line;
	ifstream f;
	f.open(fileName);

	if(!f) {
		cout << "File wasn't found." << endl;
		exit(1);
	}

	while(getline(f, line))
	{
		cout << line << endl;
	}
}

int main()
{

	string fileName;
	cout << "Introduce the name of the file that contains the graph in DIMACS format: ";
	cin >> fileName;
	
	readGraph(fileName);
}