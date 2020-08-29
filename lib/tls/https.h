#pragma once
#include<map>
#include<chrono>
#include<tcpip/server.h>

struct Host {
	std::string host;
	std::string ip;
	int port;
	int weight;
	int hit;
};

struct HostNPort : std::vector<Host>
{
	std::tuple<std::string, int> operator[](std::string host);
};

class Middle : public Server
{//use thread for multiple connection, should overload read func-> return exact one req
public:
	Middle(int outport = 4433, int time_out = 0, int queue_limit = 50, std::string end_string = "end$this_is_thie_end#$#$@@@");
	int start();
	void read_config(std::string filename);

protected:
	HostNPort hostNport_;
	bool debug_ = false;

private:
	void connected(int client_fd);
	int get_full_length(const std::string &s);
};


