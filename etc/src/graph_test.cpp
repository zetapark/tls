#include<iostream>
#include<fstream>
#include"cpp_graph.h"
using namespace std;

int main() {
	ifstream f{"sav"};
	Graph<string> g;
	f >> g;
	g.remove_edge("is", "This");
	cout << g;
	
}

