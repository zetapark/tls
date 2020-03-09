#pragma once
#include<map>
#include<mutex>
#include<chrono>
#include<shared_mutex>
#include"tcpip/server.h"

struct MClient : Client, std::mutex
{
	MClient(std::string ip = "localhost", int port = 2001);
};

struct SClient 
{
	std::chrono::system_clock::time_point issue_time;//set in new session ticket
	std::shared_ptr<MClient> sp_client;//set in psk ext or new session ticket
	std::array<std::vector<uint8_t>, 2> key, iv;//set in set_aes
};

template<class V> struct WithMutex : std::mutex, V
{ };

template<class K, class V, class Cond = int> class Smap
	: std::map<K, std::shared_ptr<WithMutex<V>>>
{//session ticket database class 
public:
	Smap() {
		if constexpr(!std::is_same<int, Cond>::value)
			th_ = std::thread{garbage_collect, this};
	}
	~Smap() {
		run_ = false;
		if(th_.joinable()) th_.join();
	}
	std::shared_ptr<WithMutex<V>> operator[](K k) const {
		std::shared_lock<std::shared_mutex> lck{mtx_};
		return find(k) == end() ? nullptr : at(k);
	}
	bool insert(K k, V v);

protected:
	std::thread th_;
	std::shared_mutex mtx_;
	bool run_ = true;

private:
	void garbage_collect() {
		std::unique_lock<std::shared_mutex> lck{mtx_, std::defer_lock};
		while(1) {
			for(int i=0; i<200; i++) {
				if(run_) this_thread::sleep_for(3s);
				else break;
			}
			lck.lock();
			auto it = std::remove_if(begin(), end(), Cond);
			erase(it, end());
			lck.unlock();
		}

	}
};
