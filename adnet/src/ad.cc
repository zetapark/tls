#include<arpa/inet.h>
#include<util/log.h>
#include"ad.h"
using namespace std;
using namespace std::chrono;

std::string base64_encode(std::vector<unsigned char> v);
std::vector<unsigned char> base64_decode(std::string s);

void Ad::process(sockaddr_in &&ip)
{
	char client_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ip.sin_addr), client_ip, INET_ADDRSTRLEN);
	//psstm(string{"geoiplookup "} + client_ip);
	if(requested_document_ == "request_ad.php") content_ = request_ad();
	else if(requested_document_ == "click_ad.php") click_ad();
}

string Ad::request_ad() 
{//do not use LOG with sq : LOGD << sq -> error
	if(last_save_ < system_clock::now() - INTERVAL * 1s) {
		if(!sq.reconnect()) sq.connect("localhost", "adnet", "adnetadnet", "adnet");
		last_save_ = system_clock::now();
		prev_token_ = move(token_);
		insert_increment();
		if(sq.query("select count(*) from Users")) sq.fetch(-1);
		int user_count = sq[0][""].asInt() / 3 + 100;//when service is new and people low
		if(sq.query("select id, link from Users where click_induce <> 0 order by"
					" (my_banner_show / click_induce) limit " + to_string(user_count))) 
			user_count = sq.fetch(-1);//real fetched lines
		uniform_int_distribution<> di{0, user_count - 1};
		di_.param(di.param());
	}
	int pick = di_(rd_);
	view_induce_[nameNvalue_["id"]]++;
	view_increase_[sq[pick]["id"].asString()]++;
	return sq[pick]["id"].asString() + '\n' + sq[pick]["link"].asString() + '\n' + new_token();
}

void Ad::click_ad()
{
	if(check_token(nameNvalue_["token"])) {//check click multiple times -- needs more
		click_induce_[nameNvalue_["id"]]++;
		click_increase_[nameNvalue_["click"]]++;
	}
}

void Ad::insert_increment()
{
	map<string, int> *p[4] = {&click_induce_, &view_induce_, &click_increase_, &view_increase_};
	const char *col_name[4] = {"click_induce", "show_induce", "my_banner_click", "my_banner_show"};
	for(int i=0; i<4; i++) if(string cases, ids; !p[i]->empty()) {
		for(const auto &[id, inc] : *p[i]) {//further implement -> check query length
			cases += " when id = '" + id + "' then " + col_name[i] + " + " + to_string(inc);
			ids += "'" + id + "',";
		}
		ids.pop_back();//remove ,
		sq.query(string{"update Users set "} + col_name[i] + " = case " + cases +
				" end where id in (" + ids + ")");
	}

	view_increase_.clear(); click_increase_.clear();
	view_induce_.clear(); click_induce_.clear();
}

string Ad::new_token()
{
	vector<unsigned char> v;
	for(int i=0; i<15; i++) v.push_back(token_di_(rd_) % 0x100);
	string r = base64_encode(v);
	token_.insert(r);
	return r;
}

bool Ad::check_token(string token)
{
	if(auto it = token_.find(token); it != token_.end()) token_.erase(it);
	else if(it = prev_token_.find(token); it != prev_token_.end()) prev_token_.erase(it);
	else return false;
	return true;
}


/*
			SELECT (6371 * acos( 
					cos( radians(lat2) ) 
					* cos( radians( lat1 ) ) 
					* cos( radians( lng1 ) - radians(lng2) ) 
					+ sin( radians(lat2) ) 
					* sin( radians( lat1 ) )
					) ) as distance from your_table
*/
