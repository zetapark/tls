#include"database/mysqldata.h"
#include"tcpip/bootstrap.h"

class DnDD : public BootStrapSite
{
public:
	DnDD();

protected:
	SqlQuery sq, sq2;
	void process();

private:
	void signin(), index(), search(), mn(), pg(), edit(), new_book(), comment(),result(), google();
	std::string search(std::string s), field(std::string s), follow(), close(), vote();
	void tut(), opencv(), insert_bcard(), busi(), crop();
	std::array<int, 5> allowlevel(std::string table, std::string book);
	std::vector<std::string> tables();
	int maxpage(std::string table, std::string book);
	std::string id, level="0", name, db, table, book, page, group, group_desc, logo,
		id2_, front_img_, back_img_;
	Json::Value tmp;
	std::array<int, 5> allow;
	string iframe_content_, namecard_;
};

