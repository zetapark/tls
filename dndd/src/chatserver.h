#include<tcpip/server.h>

class Server2 : public Server 
{//extends Server to accept server side preemptive messaging
public:
	template <typename T> void start(T f) {
		int cl_size = sizeof(client_addr);
		while(true) {
			client_fd = accept(server_fd, (sockaddr*)&client_addr, (socklen_t*)&cl_size);
			if(client_fd == -1) cout << "accept() error" << endl;
			else {//connection established
				cout << "accepting" << endl;
				if(!fork()) {//child process begin here, current fd & addr is copied
					int time_left;
					auto ff = [&](string s) {//add timer to server function
						time_left = time_out;
						return f(s);
					};
					AsyncQueue<string> aqv{bind(&T::event, &f),
						bind(&Tcpip::send, this, placeholders::_1)};
					AsyncQueue<string> aq{bind(&Tcpip::recv, this), //multi thread
						bind(&Tcpip::send, this, bind(ff, placeholders::_1))};
					//timer. aq will destroy its thread automatically when out of range
					while(time_left--) this_thread::sleep_for(1s);
				}
			}
		}
	}
};

class ChatServer 
{
public:
	std::string operator()(std::string s);

protected:
	static std::vector<std::string> rooms_;
};
