#include<regex>
#include<iostream>
#include<unistd.h>
#include<cassert>
#include<thread>
#include"tls13.h"
#include"https.h"
#include"util/log.h"
#include"cert_util.h"
using namespace std;

PSK PSKnCLIENT;

Middle::Middle(int outport, int timeout, int queue, string end)
	: Server{outport, timeout, queue, end}
{ } 
	
int Middle::get_full_length(const string &s) 
{//this make HTTP recv into TLS recv
	if(s.size() < 5) return -1;
	else if((uint8_t)s[0] < 0x14 || (uint8_t)s[0] > 0x18) return -2;//not tls message
	return static_cast<unsigned char>(s[3]) * 0x100 + static_cast<unsigned char>(s[4]) + 5;
}

void Middle::read_config(string file)
{
	ifstream f{file};
	string subdomain, ip; int port;
	while(f >> subdomain >> ip >> port) hostNport_[subdomain] = {ip, port};
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

static pair<string, string> get_hostncookie(string s)
{
	s = s.substr(0, s.find("\r\n\r\n")+4);
	smatch m1, m2;
	regex e1{R"(Host: (.+?)\r\n)"}, e2{R"(Cookie: .*eZFramework=(.+?)[;\r])"};
	regex_search(s, m1, e1);
	regex_search(s, m2, e2);
	return {m1[1].str(), m2[1].str()};
}

void Middle::connected(int fd)
{//will be used in parallel
	TLS13<SERVER> t;//TLS is decoupled from file descriptor
	if(auto cl = t.handshake(bind(&Middle::recv, this, fd),//shared pointer
			bind(&Middle::send, this, placeholders::_1, fd))) {//handshake complete
		while(1) {
			if(auto a = recv(fd)) {//optional<string> a
				if(string cookie; a = t.decode(move(*a))) {//check integrity
					if(!*cl) {//no session resumption, this part is for web server
						auto [host, id] = get_hostncookie(*a);//check html header
						if(id != "") if(auto scl = PSKnCLIENT[base64_decode(id)])
							*cl = scl->sp_client;//resume using cookie
						if(!*cl) { //first connection
							stringstream ss; ss << host;
							getline(ss, host, '.');
							auto [ip, port] = hostNport_[host];
							if(port == 0) port = 2001;
							tie(cookie, *cl) = t.new_session(ip, port, t.is_tls13());//cookie:base64 encoded id
						}
					}
					LOGT << *a << endl;
					if((*cl)->accumulate(*a)) {//multiple packets constitute one message
						if((*cl)->try_lock_for(7s)) {
							(*cl)->send();//to inner server
							a = (*cl)->recv();
							(*cl)->unlock();
						} else break;
						if(a) {
							if(cookie != "") 
								a->insert(a->find("\r\n\r\n"), "\r\nSet-Cookie: eZFramework=" + cookie);
							send(t.encode(move(*a)), fd);//to browser
						} else {//getting response from web server failed
							PSKnCLIENT.remove(*cl);//remove connection entry to errored web server
							break;
						}
					}//do not break here
				} else break;
			} else break;
		}
	}
	close(fd); 		LOGI << "closing connection " << fd << endl;
}

