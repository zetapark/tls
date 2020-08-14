#include<iostream>
#include<fstream>
#include"cpp_graph.h"
using namespace std;

int main() {
	Graph<string> g;
	ofstream f{"sav"};
	g.insert_vertex("Hello");
	g.insert_vertex("This");
	g.insert_vertex("is");
	g.insert_vertex("test");
	g.insert_edge("is", "This", 2. + 4.i);
	f << g;
}


