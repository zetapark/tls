#include<array>
#include<atomic>
#include<shared_mutex>

template<class T, int N = 1000> class ThreadSafeVector
{
public:
	T operator[](int n) {
		std::shared_lock<std::shared_mutex> lck{mtx_};
		return ar_[n];
	}
	void push_back(T val) {
		std::shared_lock<std::shared_mutex> lck{mtx_};
		ar_[pos_++ % N] = val;
	}
	int size() const {
		return pos_;
	}
	template<class Cond> int find_if(Cond cond) {//return first element that meets cond
		std::shared_lock<std::shared_mutex> lck{mtx_};
		auto it = std::find_if(ar_.begin(), ar_.begin() + pos_, cond);
		return it - ar_.begin();
	}
	int erase(int b, int e) {
		std::unique_lock<std::shared_mutex> lck{mtx_};
		std::remove_if(ar_.begin() + b, ar_.begin() + e, true);
		pos_ -= e - b;
		return pos_;
	}
	template<class Cond> void remove_if(Cond cond) {
		std::unique_lock<std::shared_mutex> lck{mtx_};
		auto it = std::remove_if(ar_.begin(), ar_.begin() + pos_, cond);
		pos_ = it - ar_.begin();
	}

protected:
	std::atomic<int> pos_{0};
	std::array<T, N> ar_;
	std::shared_mutex mtx_;
};

template<class K, class V, int N = 1000> class ThreadSafeMap
	: public ThreadSafeVector<std::pair<K, V>, N>
{
public:
	std::optional<V> operator[](K k) {
		std::shared_lock<std::shared_mutex> lck{this->mtx_};
		int position = this->pos_;
		int pos = this->find_if([k](std::pair<K, V> p) { return p.first == k; });
		return position <= pos ? std::optional<V>{} : this->ar_[pos].second;
	}
	void insert(K k, V v) {
		this->push_back({k, v});
	}
};
