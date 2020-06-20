#pragma once
#include<chrono>
#include<database/mysqldata.h>
#include<tcpip/website.h>
#include<random>

class Ad : public WebSiteIP
{
protected:
	SqlQuery sq;
	string id_;
	int hit_ = 0;
	void process(sockaddr_in &&ip);

private:
	std::map<std::string, std::chrono::time_point<std::chrono::system_clock>> token_;
	std::map<std::string, int> view_increase_, click_increase_, view_induce_, click_induce_;
	std::string request_ad(), new_token();
	bool check_token(std::string token);
	std::random_device rd_;
	std::uniform_int_distribution<> di_{0, 10};
	void insert_increment(), click_ad();
};


