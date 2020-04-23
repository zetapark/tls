#pragma once
#include"database/mysqldata.h"
#include"tcpip/website.h"

class Biz : public WebSite
{
protected:
	void process();
	SqlQuery sq;
private:
	void opencv(), insert_bcard(), index(), crop();
	std::string id_, front_img_, back_img_, namecard_;
};

class Biz2 : public WebSite
{
protected:
	void process();

};

