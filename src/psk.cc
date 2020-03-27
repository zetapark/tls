#include"psk.h"
#define DUR (9 * 256s)
using namespace std;

MClient::MClient(string ip, int port) : Client{ip, port}
{ }

bool MClient::accumulate(string s)
{
	to_send += s;
	return this->get_full_length(to_send) <= to_send.size() ? true : false;
}

void MClient::send()
{
	int full_sz = this->get_full_length(to_send);
	Client::send(to_send.substr(0, full_sz));
	to_send = to_send.substr(full_sz);
}

thread_local string MClient::to_send;

PSK::PSK() : th_{&PSK::garbage_collect, this}
{ }

PSK::~PSK() 
{
	run_ = false;
	th_.join(); 
}

void PSK::garbage_collect()
{
	while(1) {
		for(int i=0; i<200; i++) {//sleep phase
			if(run_) this_thread::sleep_for(3s);
			else return;
		}
		remove_if([](const std::pair<std::vector<uint8_t>, SClient> &p) { return
				p.second.issue_time < std::chrono::system_clock::now() - DUR; });
	}
}
