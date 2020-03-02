#include"tcpip/server.h"
#include"my_site.h"
using namespace std;


int main(int ac, char** av)
{
	Site f;
	int port = ac < 2 ? 2002 : atoi(av[1]);
	Server sv{port};
	cout << "opening port " << port << endl;
	sv.start(f);
}
