#pragma once
#include<thread>
#include<chrono>
#include"tcpip/server.h"
#include"tvec.h"

struct MClient : Client, std::mutex
{
	MClient(std::string ip = "localhost", int port = 2001);
};

struct SClient 
{
	std::chrono::system_clock::time_point issue_time;//set in new session ticket
	std::shared_ptr<MClient> sp_client;//set in psk ext or new session ticket
	std::vector<uint8_t> psk;
};

class PSK : public ThreadSafeMap<std::vector<uint8_t>, SClient>
{
public:
	PSK();
	~PSK();
protected:
	std::thread th_;
	bool run_ = true;
private:
	void garbage_collect();
};
