#pragma once
#include"database/mysqldata.h"
#include"tcpip/website.h"

class Biz : public WebSite
{
public:
	Biz() = default;
protected:
	void process();
	SqlQuery sq;
private:
	void opencv(), insert_bcard(), index(), crop();
	std::string id_, front_img_, back_img_, namecard_;
};

