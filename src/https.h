#pragma once
#include<map>
#include<chrono>
#include"tcpip/server.h"

class Middle : public Server
{//use thread for multiple connection, should overload read func-> return exact one req
public:
	Middle(int outport = 4433, int time_out = 0, int queue_limit = 50, std::string end_string = "end$this_is_thie_end#$#$@@@");
	int start();
	void read_config(std::string filename);

protected:
	std::map<std::string, int> hostNport_;
	bool debug_ = false;

private:
	void connected(int client_fd);
	int get_full_length(const std::string &s);
};


