#pragma once
#include<map>
#include<string>
#include"tcpip/shared_files.h"
using namespace std;

class WebSite 
{//specific server will derive this class
public:
	static void init(std::string dir = "www");
	std::string operator()(std::string s);

protected:
	virtual void process() {}//child should implement this
	bool swap(std::string, std::string), append(std::string, std::string),
		 prepend(std::string a, std::string b);
	std::map<std::string, std::string> nameNvalue_;//parameter
	std::string content_, requested_document_;//set content_
	static SharedMem fileNhtml_;

private:
	const std::string header_ = 
	"HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: ";
	void parse_multi(std::istream& is, std::string boundary);
	std::istream& parse_one(std::istream& is, std::string boundary);
	static std::map<std::string, std::string> parse_post(std::istream& post);
	
	friend void destroy_shared_mem(int sig);
};

