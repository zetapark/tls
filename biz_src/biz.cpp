#include"tcpip/server.h"
#include"biz.h"
using namespace std;


int main(int ac, char** av)
{
	Biz f;
	f.init("biz_html");
	int port = ac < 2 ? 2002 : atoi(av[1]);
	Server sv{port};
	cout << "opening port " << port << endl;
	return sv.start(f);
}
