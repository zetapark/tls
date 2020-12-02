#include<tcpip/server.h>
#include<util/option.h>
#include<iostream>
using namespace std;

int main()
{
	Client cl{"localhost", 5984};
	cl.send("GET /test/doc3\r\n\r\n");
Authorization: Basic zeta:cockcodk0(base64encode)

	cout << *cl.recv() << endl;
}

