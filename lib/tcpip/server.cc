#include<utility>
#include<iostream>
#include<chrono>
#include<unistd.h>
#include<netdb.h>//gethostbyname
#include<regex>
#include<sys/wait.h>
#include"server.h"
using namespace std;

Vrecv::Vrecv(int port) : Tcpip{port}
{ }


optional<string> Vrecv::recv(int fd)
{
	int len;
	static thread_local std::string trailing_string;//for thread safety
	while(!(0 < (len = get_full_length(trailing_string)) && 
				len <= trailing_string.size())) {
		if(len == -2) return {};//wrong protocol
		if(auto a = Tcpip::recv(fd)) trailing_string += *a;
		else return {};
	}
	string r = trailing_string.substr(0, len);
	trailing_string = trailing_string.substr(len);
	return r;
}

int Vrecv::get_full_length(const string& s) 
{//this should be replaced with inherent class function
	return s.size();
}

Http::Http(int port) : Vrecv{port}
{ }

int Http::get_full_length(const string &s)
{//get full length of one request. assume that s is a first received string
	smatch m;
	if(regex_search(s, m, regex{R"(Content-Length:\s*(\d+))"})) 
		return stoi(m[1].str()) + s.find("\r\n\r\n") + 4;
	else return s.size();
}

TlsLayer::TlsLayer(int port) : Vrecv{port}
{ }

int TlsLayer::get_full_length(const string& s)
{
	if(s.size() < 5) return -1;
	return static_cast<unsigned char>(s[3]) * 0x100 + static_cast<unsigned char>(s[4]) + 5;
}

Client::Client(string ip, int port) : Http(port)
{
	server_addr.sin_addr.s_addr = inet_addr(get_addr(ip).c_str());
	if(-1 == connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)))
		cout << "connect() error" << endl;
	else cout << "connecting to " << ip << ':' << port  <<endl;

	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
}

string Client::get_addr(string host)
{///get ip from dns
	auto* a = gethostbyname(host.data());
	return inet_ntoa(*(struct in_addr*)a->h_addr);
}

static void kill_zombie(int) {
	int status;
	waitpid(-1, &status, WNOHANG); 
}

Server::Server(int port, unsigned int t, int queue, string e) : Http(port) 
{
	end_string = e;
	time_out = t;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int enable = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	cout << "setsockopt(SO_REUSEADDR) failed";
	if(bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		ok_ = false;
		cout << "bind() error" << endl;
	} else cout << "binding" << endl;
	if(listen(server_fd, queue) == -1) {
		ok_ = false;
		cout << "listen() error" << endl;
	} else cout << "listening port " << port << endl;

	struct sigaction sa;
	sa.sa_handler = kill_zombie;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGCHLD, &sa, 0);
}

int Server::start(function<string(string)> f)
{
	if(!ok_) return 1;
	int cl_size = sizeof(client_addr);
	while(1) {
		client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
		if(time_out) {//set socket option
			struct timeval tv;
			tv.tv_sec = time_out;
			tv.tv_usec = 0;
			setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
			tv.tv_sec = 3;
			setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof tv);
			bool keep_alive = true;
			setsockopt(client_fd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive));
		}
		if(client_fd == -1) cout << "accept() error" << endl;
		else if(!fork()) {//string size 0 : error -> s.size() : verify 
			for(optional<string> s; s = recv(); send(f(*s)));//recv server fail 시 에러
			send(end_string);
			close(client_fd);
			break;//forked process ends here
		}
	}
	return 0;
}

int Server::nokeep_start(function<string(string)> f)
{//does not keep connection
	int cl_size = sizeof(client_addr);
	while(true) {
		client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
		if(client_fd == -1) cout << "accept() error" << endl;
		else {//connection established
			cout << "accepting" << endl;
			send(f(*recv()));
			close(client_fd);
		}
	}
	return 0;
}
