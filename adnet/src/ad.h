#pragma once
#include<chrono>
#include<database/mysqldata.h>
#include<tcpip/website.h>
#include<random>
#include<set>

class Ad : public WebSiteIP
{
protected:
	SqlQuery sq;
	void process(sockaddr_in &&ip);

	void click_ad();
	std::string request_ad();
	
private:
	void insert_increment();
	bool check_token(std::string token);
	std::string new_token();

	std::chrono::time_point<std::chrono::system_clock> last_save_ = std::chrono::system_clock::now();
	std::set<std::string> token_, prev_token_;
	std::map<std::string, int> view_increase_, click_increase_, view_induce_, click_induce_;
	std::uniform_int_distribution<> di_{0, 10}; std::random_device rd_;
};


