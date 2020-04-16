#pragma once
#include<thread>
#include<chrono>
#include<mutex>
#include"tcpip/server.h"
#include"tvec.h"

struct MClient : Client, std::timed_mutex
{
	MClient(std::string ip = "localhost", int port = 2001);
	bool accumulate(std::string s);
	static thread_local std::string to_send;
	void send();
};

struct SClient 
{
	std::chrono::system_clock::time_point issue_time;//set in new session ticket
	std::shared_ptr<MClient> sp_client;//set in psk ext or new session ticket
	std::vector<uint8_t> psk;//in 1.2 this is master secret
};

class PSK : public ThreadSafeMap<std::vector<uint8_t>, SClient>
{//key 1.2 : session id, 1.3 : ticket id
public:
	PSK();
	~PSK();
protected:
	std::thread th_;
	bool run_ = true;
private:
	void garbage_collect();
};
