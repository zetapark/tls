#include<tcpip/website.h>

class Misc : public WebSiteIP
{
protected:
	void process(sockaddr_in&&);
};

