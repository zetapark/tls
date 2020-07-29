#pragma once
#include<map>
#include<functional>
#include<mutex>
#include<thread>
#include"tcpip.h"

class Vrecv : public Tcpip
{//virtual class that provide interface to get recv work just as expected
public:
	Vrecv(int port);
	std::optional<std::string> recv(int fd=0);
	//check content length header and get one full request
protected:
	virtual int get_full_length(const std::string& s);//define this to make recv adapt to environment
};

class Http : public Vrecv
{
public:
	Http(int port);

protected:
	int get_full_length(const std::string& s);
};

class TlsLayer : public Vrecv
{
public:
	TlsLayer(int port);

protected:
	int get_full_length(const std::string& s);
};

class Client : public Http
{
public:
	Client(std::string ip = "127.0.0.1", int port = 2001); ///<constructor
private:
	std::string get_addr(std::string host);
};

class Server : public Http
{
public:
	Server(int port = 2001, unsigned int time_out = 60*60*5, int queue_limit = 50,
			std::string end_string = "end");
	int start(std::function<std::string(std::string)> f);
	int nokeep_start(std::function<std::string(std::string)> f);

	template<class F> int keep_start(F &f)
	{//all connections share one server state (for adnet)
		int cl_size = sizeof(client_addr);
		std::vector<std::thread> v;
		std::mutex mtx;
		auto lambda = [&](int fd) {
			std::unique_lock lck{mtx, std::defer_lock};
			while(auto a = recv(fd)) {
				lck.lock();
				a = f(*a);
				lck.unlock();
				send(*a, fd);
			}
			close(fd);
		};
		while(true) {
			client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
			if(client_fd != -1) {//connection established
				v.emplace_back(std::thread{lambda, client_fd});//this is not the real ip, 
				v.back().detach();
			}
		}
		return 0;
	}

protected:
	std::string end_string;
	int time_out;
	bool ok_ = true;
};

