#include<iostream>
#include<unistd.h>
#include<cassert>
#include<thread>
#include"tls13.h"
#include"https.h"
#include"util/log.h"
using namespace std;

Middle::Middle(int outport, int inport, int timeout, int queue, string end)
	: Server{outport, timeout, queue, end}, inport_{inport}
{//hI = this; 
	LOGI << "opening inner port " << inport << endl;
} 
	
int Middle::get_full_length(const string &s) 
{//this make HTTP recv into TLS recv
	if(s.size() < 5) return -1;
	else if((uint8_t)s[0] < 0x14 || (uint8_t)s[0] > 0x18) return -2;//not tls message
	return static_cast<unsigned char>(s[3]) * 0x100 + static_cast<unsigned char>(s[4]) + 5;
}

int Middle::start()
{//middle server can be managed here
	if(!ok_) return 1;
	int cl_size = sizeof(client_addr);
	vector<thread> v;
	while(1) {
		client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
		LOGI << "accepting " << client_fd << " fd" << endl;
		if(client_fd == -1) LOGF << "accept() error" << endl;
		else {//fork가 아니므로 client_fd가 변함.
			v.emplace_back(thread{&Middle::connected, this, client_fd});
			v.back().detach();
		}
	}
}

void Middle::connected(int fd)
{//will be used in parallel
	TLS13<SERVER> t;//TLS is decoupled from file descriptor
	if(auto cl = t.handshake(bind(&Middle::recv, this, fd),//shared pointer
			bind(&Middle::send, this, placeholders::_1, fd), inport_)) {
		//Client cl{"localhost", inport_};
		while(1) {
			if(auto a = recv(fd)) {//optional<string> a
				if(a = t.decode(move(*a))) {
					LOGT << *a << endl;
					if(cl->accumulate(*a)) {
						cl->lock();
						cl->send();//to inner server
						a = cl->recv();
						cl->unlock();
						if(a) send(t.encode(move(*a)), fd);//to browser
						else break;
					}
				} else break;
			} else break;
		}
	}
	close(fd); 		LOGI << "closing connection " << fd << endl;
}

