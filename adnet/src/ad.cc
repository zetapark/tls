//#include<arpa/inet.h>
//#include<util/log.h>
#include<cmath>
#include<database/util.h>
#include"ad.h"
using namespace std;
using namespace std::chrono;

std::string base64_encode(std::vector<unsigned char> v);
std::vector<unsigned char> base64_decode(std::string s);

void Ad::process()
{
	if(th_.joinable()) th_.join();//from request ad function
//	char client_ip[INET_ADDRSTRLEN];
	//psstm(string{"geoiplookup "} + client_ip);
	if(requested_document_ == "request_ad.php") content_ = request_ad();
	else if(requested_document_ == "click_ad.php") click_ad();
}

namespace {
tuple<string, float, float> get_position(string ip)
{
	stringstream ss; float lat, lng; char c; string nation, s;
	ss << psstm("geoiplookup " + ip.substr(0, ip.find(':')));
	getline(ss, s, ':');
	ss >> nation; nation.pop_back();
	getline(ss, s); getline(ss, s, ':'); for(int i=0; i<5; i++) getline(ss, s, ',');
	ss >> lat >> c >> lng;
	return {nation, lat, lng};
}

float distance(float lata, float lnga, float latb, float lngb)
{
	lata = lata * M_PI / 180;
	lnga = lnga * M_PI / 180;
	latb = latb * M_PI / 180;
	lngb = lngb * M_PI / 180;
	return 6371 * acos(cos(latb) * cos(lata) * cos(lnga - lngb) + sin(latb) * sin(lata));
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

vector<string> divide_category(string cat)
{
	vector<string> r; string s;
	stringstream ss; ss << cat;
	while(ss >> s) r.push_back(s);
	return r;
}
}

string Ad::request_ad() 
{//do not use LOG with sq : LOGD << sq -> error
	float lat, lng; string nation;
	tie(nation, lat, lng) = get_position(nameNvalue_["IP-Addr"]);

	int pick = 0;//category have priority : 3 point
	for(int i=0, current_best_point=0, point=0; i<sq.size(); i++) {
		if(!sq[i]["country"].asBool() || sq[i]["nation"].asString() == nation) point += 2;
		if(!sq[i]["distance"].asBool() || distance(lat, lng, sq[i]["lat"].asFloat(),
					sq[i]["lng"].asFloat()) < sq[i]["km"].asInt()) point += 2;
		if(string s = nameNvalue_["category"]; s == "") point += 3;
		else for(string t : divide_category(s)) if(sq[i][t].asBool()) { point += 3; break; }
		if(point == 7) { pick = i; break; }
		else if(current_best_point < point) current_best_point = point, pick = i;
	}
	view_induce_[nameNvalue_["id"]]++;
	view_increase_[sq[pick]["id"].asString()]++;
	sq.removeIndex(pick, &sq[pick]);
	if(sq.empty()) th_ = thread{&Ad::all_the_database_job, this};
	return sq[pick]["id"].asString() + '\n' + sq[pick]["link"].asString() + '\n' + new_token();
}

void Ad::click_ad()
{
	if(check_token(nameNvalue_["token"])) {//check click multiple times -- needs more
		click_induce_[nameNvalue_["id"]]++;
		click_increase_[nameNvalue_["click"]]++;
	}
}

void Ad::all_the_database_job()
{
	if(!sq.reconnect()) sq.connect("localhost", "adnet", "adnetadnet", "adnet");
	insert_increment();
	prev_token_ = move(token_);
	if(sq.query("select count(*) from Users")) sq.fetch(-1);
	int user_count = sq[0][""].asInt() / 5 + 100;//when service is new and people low
	sq.query("select * from Pref right join "//joined id -> right table will be the value
			"(select id, link, nation, lat, lng, km, etc from Users where click_induce <> 0 "
			"order by etc, (my_banner_show / click_induce) limit " + to_string(user_count)
			+ ") as t1 on t1.id = Pref.id");//etc 1: postponed banner for coarse quality, 0: ok
	sq.fetch(-1);//real fetched lines
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
	uniform_int_distribution<> di{0, 255};
	for(int i=0; i<15; i++) v.push_back(di(rd_) % 0x100);
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


