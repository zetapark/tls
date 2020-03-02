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
	
void Middle::conn()
{
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

int Middle::start()
{//middle server can be managed here
	if(!ok_) return 1;
	thread th{&Middle::conn, this};
	th.detach();
	while(1) this_thread::sleep_for(10s);
}

void Middle::connected(int fd)
{//will be used in parallel
	TLS13<SERVER> t;//TLS is decoupled from file descriptor
	if(t.handshake(bind(&Middle::recv, this, fd),
			bind(&Middle::send, this, placeholders::_1, fd))) {
		Client cl{"localhost", inport_};
		while(1) {
			if(auto a = recv(fd)) {
				if(a = t.decode(move(*a))) cl.send(*a);//to inner server
				else break;
				if(auto b = cl.recv()) send(t.encode(move(*b)), fd);//to browser
				else break;
			} else break;
		}
	}
	close(fd); 		LOGI << "closing connection " << fd << endl;
}

