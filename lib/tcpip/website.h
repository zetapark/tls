#pragma once
#include<map>
#include<string>
#include<vector>
#include<netinet/in.h>//sockaddr_in
#include"tcpip/shared_files.h"
using namespace std;

class WebSite 
{//specific server will derive this class
public:
	static void init(std::string dir = "www");
	std::string operator()(std::string s);
	static void add_header(std::string re, std::string he);

protected:
	virtual void process() {}//child should implement this
	bool swap(std::string, std::string), append(std::string, std::string),
		 prepend(std::string a, std::string b);
	std::map<std::string, std::string> nameNvalue_, req_header_;//parameter
	std::string content_, requested_document_;//set content_
	static SharedMem fileNhtml_;
	void parse_all(string &&s);
	std::string return_content();
	std::string header();

private:
	static std::multimap<std::string, std::string> added_header_;
	void parse_multi(std::istream& is, std::string boundary);
	std::istream& parse_one(std::istream& is, std::string boundary);
	static std::map<std::string, std::string> parse_post(std::istream& post);
	
	friend void destroy_shared_mem(int sig);
};

std::string carousel(std::vector<std::string> img, std::vector<std::string> desc, std::vector<std::string> href);

