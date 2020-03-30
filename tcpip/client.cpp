#include<iostream>
#include"util/option.h"
#include"tcpip/server.h"
using namespace std;

int main(int ac, char **av) {
	CMDoption co{
		{"ip", "ip address", "localhost"},
		{"port", "listening port", 4433},
		{"string", "string to send", ""},
		{"log filter", "log filter setting, use lower case to disable log", "tdIWEF"}
	};
	if(!co.args(ac, av)) return 0;
	Client cl{co.get<const char*>("ip"), co.get<int>("port")};
	cl.send(co.get<const char*>("string"));
	cout << *cl.recv() << endl;
}

