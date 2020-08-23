#pragma once
#include<thread>
#include<chrono>
#include<mutex>
#include<tcpip/server.h>
#include"tvec.h"

struct MClient : Client, std::timed_mutex
{
	MClient(std::string ip = "localhost", int port = 2001, int fd = 0);
	union {
		uint32_t real_ip;
		uint8_t a[4];
	} u;
	unsigned short port;
	bool accumulate(std::string s);
	static thread_local std::string to_send;
	int send();
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
	void remove(std::shared_ptr<MClient> cl);
	std::optional<SClient> operator[](std::vector<uint8_t> key);
protected:
	std::thread th_;
	bool run_ = true;
private:
	void garbage_collect();
};
