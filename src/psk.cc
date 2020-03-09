#include"psk.h"
using namespace std;

template<class K, class V> V Map::operator[](const K &k) const
{
	shared_lock<shared_mutex> lck{mtx_, 3s};
	return map::operator[](k);
}

template<class K, class V> V& Map::insert(K k, V v)
{
	unique_lock<shared_mutex> lck{mtx_, 3s};
	return map::operator[](k);
}

