#include<csignal>
#include"tcpip/server.h"
#include"dndd.h"
#include"tcpip/shared_files.h"
using namespace std;

extern SharedMem fileNhtml_;
static void destroy_shared_mem(int sig)
{
	fileNhtml_.destroy();
	cout << "destroyed shared map" << endl;
	exit(sig);
}

int main(int ac, char** av)
{
	signal(SIGINT, &destroy_shared_mem);
	DnDD f;
	int port = ac < 2 ? 2001 : atoi(av[1]);
	Server sv{port};
	cout << "opening port " << port << endl;
	return sv.start(f);
}
