#include<complex>
#include<vector>
#include<forward_list>
#include<set>
#include<map>

struct Edge {
	int index;
	int v = 0;
	std::complex<double> weight;
};

template<class T> struct Vertex {
	T data;
	std::forward_list<Edge> edges;
};

template<class T> struct Graph {
	std::vector<Vertex<T>> vertexes;
	std::set<int> deleted;//do not remove vector -> edge index unusable, just set this
	std::map<int, int> visited;//index and v value : use for vertex visit check and so on
	int insert_vertex(T n) {//return index of inserted vertex
		Vertex<T> v; v.data = n;
		vertexes.push_back(v);
		return vertexes.size() - 1;
	}
	int find_vertex(T n) {
		for(int i=0; i<vertexes.size(); i++)
			if(vertexes[i].data == n && deleted.find(i) == deleted.end()) return i;
		return -1;
	}
	void remove_vertex_with_index(int n) {
		deleted.insert(n);
	}
	void remove_vertex(T n) {//if duplicate doesn't exist, use this
		remove_vertex_with_index(find_vertex(n));
	}
	void insert_edge_with_index(int a, int b, std::complex<double> w, int v = 0) {
		Edge e;
		e.index = b; e.weight = w; e.v = v;
		vertexes[a].edges.push_front(e);
	}
	void insert_edge(T a, T b, std::complex<double> w) {
		insert_edge_with_index(find_vertex(a), find_vertex(b), w);
	}
	void remove_edge_widh_index(int a, int b) {
		vertexes[a].edges.remove_if([b](const Edge &e){return e.index == b;});
	}
	void remove_edge(T a, T b) {
		remove_edge_widh_index(find_vertex(a), find_vertex(b));
	}
};

template<class T> std::ostream &operator<<(std::ostream &os, const Graph<T> &r) {
	os << r.vertexes.size() << ' ';
	for(auto [a, b] : r.visited) os << a << ' ' << b << ' ';
	os << -1 << ' ' << -1 << ' ';//-1 terminator
	for(int i : r.deleted) os << i << ' ';
	os << -1 << '\n';
	for(int i=0; i < r.vertexes.size(); i++) os << r.vertexes[i].data << '\n';
	os << '\n';
	for(int i=0; i < r.vertexes.size(); i++) for(auto a : r.vertexes[i].edges)
		os << i << ' ' << a.index << ' ' << a.weight << ' ' << a.v << '\n';
	return os;
}

template<class T> std::istream &operator>>(std::istream &is, Graph<T> &r) {
	int sz, i, j, v;
	is >> sz;
	for(;;) {
		is >> i >> v;
		if(i == -1) break;
		r.visited[i] = v;
	}
	for(;;) {
		is >> i;
		if(i == -1) break;
		r.deleted.insert(i);
	}
	T data;
	for(i=0; i<sz; i++) {
		is >> data;
		r.insert_vertex(data);
	}
	
	for(std::complex<double> wt; is >> i >> j >> wt >> v;)
		r.insert_edge_with_index(i, j, wt, v);
	return is;
}
