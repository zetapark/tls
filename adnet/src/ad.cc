#include<cmath>
#include<database/util.h>
#include"ad.h"
#define ROUND 10
using namespace std;
using namespace std::chrono;

std::string base64_encode(std::vector<unsigned char> v);

Ad::Ad()
{
	sq.connect("localhost", "adnet", "adnetadnet", "adnet");
	sq.select("Url", "");
	for(int i=0; i<sq.size(); i++) urlNid_[sq[i]["url"].asString()] = sq[i]["id"].asString();
	all_the_database_job();
}

void Ad::process()
{
	if(requested_document_ != "adnet.js" && th_.joinable()) th_.join();//from request ad function
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
	tie(nation, lat, lng) = get_position(req_header_["IP-Addr"]);

	static int call = 0;
	int pick = 0;//priority : round(100) -> category(3) -> country, distance(2)
	for(int i=0, current_best_point=0, point=0; i<sq.size(); i++, point=0) {
		point += rounds_[i] * 100;
		if(!sq[i]["country"].asBool() || sq[i]["nation"].asString() == nation) point += 2;
		if(!sq[i]["distance"].asBool() || distance(lat, lng, sq[i]["lat"].asFloat(),
					sq[i]["lng"].asFloat()) < sq[i]["km"].asInt()) point += 2;
		if(string s = nameNvalue_["category"]; s == "") point += 3;
		else for(string t : divide_category(s)) if(sq[i][t].asBool()) { point += 3; break; }
		if(point == round_ * 100 + 7) { pick = i; break; }
		else if(current_best_point < point) current_best_point = point, pick = i;
	}
	string id = nameNvalue_["id"], url = nameNvalue_["url"];
	url = url.substr(0, url.find('?'));
	url = url.substr(0, url.find('#'));
	view_induce_[id]++;
	view_increase_[sq[pick]["id"].asString()]++;
	rounds_[pick]--;
	if(urlNid_.find(url) == urlNid_.end() || urlNid_[url] != id) url_add_[url] = id;
	string r = sq[pick]["id"].asString() + '\n' + sq[pick]["link"].asString() + '\n' + new_token();
	if(++call == sq.size()) { 
		call = 0;
		if(--round_ == 0) th_ = thread{&Ad::all_the_database_job, this};
	}
	return r;
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
	insert_url();
	prev_token_ = move(token_);
	if(sq.query("select count(*) from Users")) sq.fetch(-1);
	int user_count = sq[0][""].asInt() / 5 + 100;//when service is new and people low
	sq.query("select * from Pref right join "//joined id -> right table will be the value
			"(select id, link, nation, lat, lng, km, etc from Users where click_induce <> 0 "
			"order by etc, (my_banner_show / click_induce) limit " + to_string(user_count)
			+ ") as t1 on t1.id = Pref.id");//etc 1: postponed banner for coarse quality, 0: ok
	sq.fetch(-1);//real fetched lines
	rounds_.resize(sq.size());
	for(int &i : rounds_) i = ROUND;
	round_ = ROUND;
}

void Ad::insert_url()
{
	if(!url_add_.empty()) {
		string command = "insert into Url (url, id) values ";
		for(const auto &[url, id] : url_add_) {
			command += "('" + url + "', '" + id + "'),";
			urlNid_[url] = id;
		}
		command.pop_back();
		command += " on duplicate key update id = values(id)";
		sq.query(command);
		url_add_.clear();
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
		p[i]->clear();
	}
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


