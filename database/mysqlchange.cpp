#include"mysqldata.h"
using namespace std;

string base64_encode(vector<unsigned char> v);
int main()
{//made this to make dndd main pages base64 encoded
	SqlQuery sq;
	sq.connect("localhost", "dndd", "dndddndd", "dndd");
	for(const char *p : {"수능", "공지사항", "VotingBoard", "Sample"}) {//table names
		sq.select(p, "where title <> '코멘트임.' and page <> 0 order by num, page, edit desc");
		sq.group_by({"num", "page"});//this does not interact with database server
		int k = 1;
		for(const auto &a : sq) {
			string s = a["contents"].asString();
			vector<uint8_t> v{s.begin(), s.end()};
			s = base64_encode(v);
			cout << k++ << " : ";
			sq.insert(a["num"].asInt(), a["page"].asInt(), a["email"].asString(), a["title"].asString(), s, a["date"].asString(), a["edit"].asInt() + 1);
		}
	}
}

