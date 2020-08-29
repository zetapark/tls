#include"psk.h"
#define DUR (60 * 60s)
using namespace std;

MClient::MClient(string ip, int port, int fd) : Client{ip, port}
{
	sockaddr_in addr;
	socklen_t sz = sizeof(sockaddr);

	getpeername(fd, (sockaddr*)&addr, &sz);
	u.real_ip = addr.sin_addr.s_addr;
	port = addr.sin_port;
}

bool MClient::accumulate(string s)
{
	to_send += s;
	return this->get_full_length(to_send) <= to_send.size() ? true : false;
}

int MClient::send()
{
	int full_sz = this->get_full_length(to_send);
	string s = to_send.substr(0, full_sz);
	s.insert(s.find("\r\n\r\n"), "\r\nIP-Addr: " + to_string(u.a[0]) + '.' + to_string(u.a[1])
			+ '.' + to_string(u.a[2]) + '.' + to_string(u.a[3]) + ':' + to_string(port));
	to_send = to_send.substr(full_sz);
	return Client::send(s);
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

void PSK::remove(shared_ptr<MClient> cl)
{
	std::shared_lock<std::shared_mutex> lck{this->mtx_};
	for(int i=0; i<this->pos_; i++)
		if(this->ar_[i].second.sp_client == cl) this->ar_[i].second.issue_time -= 100000s;
}

optional<SClient> PSK::operator[](vector<uint8_t> key)
{
	auto a = ThreadSafeMap<vector<uint8_t>, SClient>::operator[](key);
	if(a && chrono::system_clock::now() < a->issue_time + DUR) return a;
	else return {};
}
