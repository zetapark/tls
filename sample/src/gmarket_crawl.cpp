#include<tcpip/server.h>
#include<iostream>
using namespace std;

int main()
{
	Client cl{"192.168.0.20", 3004};
	cl.send("https://www.zeta2374.com");
	cout << *cl.recv() << endl;
}

