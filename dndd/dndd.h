#include"database/mysqldata.h"
#include"framework/bootstrap.h"

class DnDD : public BootStrapSite
{
public:
	DnDD();

protected:
	SqlQuery sq;
	void process();

private:
	void signin(), index(), search(), mn(), pg(), edit(), new_book(), comment(),result(), google();
	std::string search(std::string s), field(std::string s), follow(), close(), vote();
	std::array<int, 5> allowlevel(std::string table, std::string book);
	std::vector<std::string> tables();
	int maxpage(std::string table, std::string book);
	std::string id, level="0", name, db, table, book, page, group, group_desc, logo;
	Json::Value tmp;
	std::array<int, 5> allow;
	string iframe_content_;
};

