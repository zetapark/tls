#include<utility>
#include<iostream>
#include<thread>
#include<chrono>
#include<unistd.h>
#include<netdb.h>//gethostbyname
#include<regex>
#include<sys/wait.h>
#include"server.h"
using namespace std;

Vrecv::Vrecv(int port) : Tcpip{port}
{ }

string Vrecv::recv(int fd)
{
	string s;
	if(trailing_string_ == "") s = Tcpip::recv(fd);
	s = trailing_string_ + s;
	trailing_string_ = "";
	int len;
	for(int n; !(len = get_full_length(s));) s += Tcpip::recv(fd);
	// s += string(buffer, n))
		//n = read(client_fd, buffer, BUF_SIZE);//when less than header came
	if(len < s.size()) {//two packet once
		trailing_string_ = s.substr(len);
		s = s.substr(0, len);
	} else if(len > s.size()) {//more to come
		for(int n; s.size() < len; s += string(buffer, n))
			n = read(fd ? fd : client_fd, buffer, min(BUF_SIZE, (int)(len - s.size())));
	}
	return s;
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
	return static_cast<unsigned char>(s[3]) * 0x100 + static_cast<unsigned char>(s[4]) + 5;
}

Client::Client(string ip, int port) : Http(port)
{
	server_addr.sin_addr.s_addr = inet_addr(get_addr(ip).c_str());
	if(-1 == connect(client_fd, (sockaddr*)&server_addr, sizeof(server_addr)))
		cout << "connect() error" << endl;
	else cout << "connecting to " << ip << ':' << port  <<endl;
}

string Client::get_addr(string host)
{///get ip from dns
	auto* a = gethostbyname(host.data());
	return inet_ntoa(*(struct in_addr*)a->h_addr);
}

Server::Server(int port, unsigned int t, int queue, string e) : Http(port) 
{
	end_string = e;
	time_out = t;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
		cout << "bind() error" << endl;
	else cout << "binding" << endl;
	if(listen(server_fd, queue) == -1) cout << "listen() error" << endl;
	else cout << "listening port " << port << endl;
}

void Server::start(function<string(string)> f)
{
	int cl_size = sizeof(client_addr);
	while(1) {
		client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
		struct timeval tv;
		tv.tv_sec = time_out;
		tv.tv_usec = 0;
		setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof tv);
		if(client_fd == -1) cout << "accept() error" << endl;
		else if(!fork()) {//string size 0 : error -> s.size() : verify 
			for(string s; (s = recv()) != end_string && s.size(); send(f(s)));//recv server fail 시 에러
			send(end_string);
			break;//forked process ends here
		}
	}
}

void Server::nokeep_start(function<string(string)> f)
{//does not keep connection
	int cl_size = sizeof(client_addr);
	while(true) {
		client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
		if(client_fd == -1) cout << "accept() error" << endl;
		else {//connection established
			cout << "accepting" << endl;
			send(f(recv()));
		}
	}
}

