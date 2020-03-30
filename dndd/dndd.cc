#include<cctype>
#include<cassert>
#include<fstream>
#include<regex>
#include"cvmatrix.h"//cvmatrix.h should come earlier than ocr.hpp
#include<opencv2/text/ocr.hpp>
#include"src/cert_util.h"
#include"dndd.h"
#include"database/util.h"
#include"tcpip/server.h"
using namespace std;

DnDD::DnDD() : BootStrapSite{"www"}
{//SqlQuery destructor -> mysqlquery destructor nullify
	sq.connect("localhost", "dndd", "dndddndd", "dndd");//sq prohibit destruct
}

void DnDD::process()
{
	cout << requested_document_ << endl;
	for(auto& a : nameNvalue_) cout << a.first << ':' << a.second << endl;
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "main.html") mn();
	else if(requested_document_ == "signin.html") signin();
	else if(requested_document_ == "page.html") pg();
	else if(requested_document_ == "edit.html") edit();
	else if(requested_document_ == "new.html") new_book();
	else if(requested_document_ == "comment.html") comment();
	else if(requested_document_ == "vote") content_ = vote();
	else if(requested_document_ == "result.html") result();
	else if(requested_document_ == "follow") content_ = follow();
	else if(requested_document_ == "search") content_ = search(nameNvalue_["search"]);
	else if(requested_document_ == "close") content_ = close();
	else if(requested_document_ == "googleapi") google();
	else if(requested_document_ == "iframe-content.html") content_ = iframe_content_;
	else if(requested_document_ == "result_view") tut();
	else if(requested_document_ == "opencv.html") opencv();
}

static string get_email(vector<string> &v)
{
	auto it = find_if(v.cbegin(), v.cend(), [](string s) { 
			return regex_match(s, regex{R"(\S+@\S+\.\S?)"}); }); 
	string r;
	if(it != v.cend()) {
		r = *it;
		v.erase(it);
	} 
	return r;
}

static string get_tel(vector<string> &v)
{//use this 3 times, usually mobile comes first, fax comes last
	auto it = find_if(v.cbegin(), v.cend(), [](string s) {
			return s.size() * 0.4 < count_if( s.begin(), s.end(), [](char c) {
					return '0' <= c && c <= '9'; });//number count
		});
	string r;
	if(it != v.cend()) {
		r = *it;
		v.erase(it);
	} 
	return r;
}

static string get_addr(vector<string> &v)
{
	auto it = std::max_element(v.cbegin(), v.cend(), [](string a, string b) {
			return a.size() < b.size();});
	string r = *it;
	v.erase(it);
	return r;
}

static string get_name(vector<string> &v)
{
	auto it = min_element(v.cbegin(), v.cend(), [](string a, string b) {
			return a.size() < b.size(); });
	string r = *it;
	cout << "이름은" << r <<  "." << endl;
	v.erase(it);
	return r;
}

static float wordvec(string kor, string eng, string s)
{
	string to_send;
	int korean = count_if(s.begin(), s.end(), [](char c) { return isprint(c) == 0;});
	stringstream ss1; ss1 << s;
	while(ss1 >> s) to_send += s + ' ' + (korean > 2 ? kor : eng) + ' ';
	Client cl{"localhost", 3003};
	if(!to_send.empty()) cl.send(to_send);
	stringstream ss;
	ss << *cl.recv();
	vector<float> vf;
	for(float f; ss >> f;) vf.push_back(f);
	vf.erase(std::remove(vf.begin(), vf.end(), 0));
	float sum = 0;
	for(auto f : vf) sum += f;
	return sum / vf.size();
}

static string get_with_wordvec(string kor, string eng, vector<string> &v)
{
	auto it = max_element(v.begin(), v.end(), [kor, eng](string a, string b) { return 
			wordvec(kor, eng, a) < wordvec(kor, eng, b); });
	string r = *it;
	v.erase(it);
	return r;
}

void DnDD::opencv() {
	vector<uint8_t> v{nameNvalue_["file"].begin(), nameNvalue_["file"].end()};
	cv::Mat mat{v, true};
	CVMat m{cv::imdecode(mat, 1)};
	m.get_business_card();

	auto a = cv::text::OCRTesseract::create(NULL, "eng+kor");
	string s;
	vector<cv::Rect> vr; vector<string> vs; vector<float> vf;
	a->run(m, s, &vr, &vs, &vf);
	swap("@DATA", s);
	vs.clear();
	stringstream ss; ss << s;
	while(getline(ss, s)) if(s != "") vs.push_back(s);
	auto it = remove_if(vs.begin(), vs.end(), [](string s) { return 
			find_if(s.begin(), s.end(), [](char c) { return !isspace(c);}) == s.end(); });
	vs.erase(it, vs.end());
	
	append("name='email'", " value='" + get_email(vs) + "' ");
	append("name='mobile'", " value='" + get_tel(vs) + "' ");
	append("name='tel'", " value='" + get_tel(vs) + "' ");
	append("name='fax'", " value='" + get_tel(vs) + "' ");
	append("name='job'", " value='" + get_with_wordvec("직장", "company", vs) + "' ");
	append("name='role'", " value='" + get_with_wordvec("직책", "role", vs) + "' ");
	append("name='addr1'", " value='" + get_addr(vs) + "' ");
	append("name='addr2'", " value='" + get_with_wordvec("주소", "address", vs) + "' ");
	append("name='name'", " value='" + get_name(vs) + "' ");
	for(int i=1; i<=vs.size() && i <= 3; i++)
		append("name='memo" + string{i} + "'", " value='" + vs[i-1] + "' ");

//	for(int i=0; i<vr.size(); i++) {
//		imshow(vs[i] + to_string(vf[i]), t(vr[i]));
//		cout << vr[i] <<' '<< vf[i] <<' '<< vs[i] << endl;
//	}

	cv::imencode(".jpg", m, v);
	namecard_ = base64_encode(v);
	swap("@IMG2", namecard_);
}

void DnDD::tut() {
	if(!nameNvalue_.empty()) {
		content_ = "<html><h2>";
		for(auto [a, b] : nameNvalue_) content_ += a + ':' + b + "<br>";
		content_ += "</h2></html>";
		cout << content_ << endl;
	}
}

void DnDD::google() 
{
	Json::Value jv; 
	Json::Reader reader;
	reader.parse(nameNvalue_["json"].data(), jv);
	ofstream f("/tmp/jv", ofstream::app|ofstream::out);
	f << jv;
	content_ = "ok";
	if(nameNvalue_["direction"] != "") {
		auto j = jv["routes"][0]["legs"];
		int sum = 0;
		for(int i=0; i<j.size(); i++) sum += j[i]["duration"]["value"].asInt();
		cout << "total " << sum << "sec" << endl;
		cout << jv["route"][0]["waypoint_order"] << endl;
	}
}

string DnDD::close()
{
	if(id == "") return "login first";
	sq.select(table, "where num=" + book + " and page=0 and title <> '코멘트임.' order by date, edit desc limit 1");
	if(id != sq[0]["email"].asString()) return "you do not own this discussion";
	string s = sq[0]["contents"].asString();
	s[1] = '5'; s[3] = '5'; allow[1] = 5; allow[3] = 5;
	sq[0]["contents"] = s;
	sq[0]["edit"] = "null";
	sq.insert(0);
	return "Discussion closed";
}	

void DnDD::result()
{
	string s = psstm("python dndd.py " + nameNvalue_["table"] + ' ' + nameNvalue_["book"] + ' ' + nameNvalue_["option"] + ' ' + nameNvalue_["db"]);
	swap("GRAPH", s);
	while(swap("dataframe\"", "dataframe table\""));
}

string DnDD::follow()
{
	if(id == "") return "login first";
	if(stoi(level) > 2) return "representative cannot follow"; 

	sq.select("Users", "where email='" + nameNvalue_["follow"] + "' order by date desc limit 1");
	if(sq[0]["level"] < 3) return "you can only follow representatives";

	sq.select("Follow", "limit 1");
	cout << nameNvalue_["secret"] << endl;
	sq.insert({nameNvalue_["follow"], id, 
			nameNvalue_["secret"] == "true" ? "1" : "0", sq.now()});
	return "follow complete";
}

string DnDD::vote()
{
	cout << "level is " << level << endl;
	if(stoi(level) < allow[3]) return "your level does not qualify";
	if(nameNvalue_["option"] == "") return "choose one";
	if(nameNvalue_["secret"] == "on" && stoi(level) > 2) 
		return "registered cannot vote secretly";
	sq.select("Vote", "limit 1");
	sq.insert({table, book, id, nameNvalue_["option"], 
			nameNvalue_["secret"] == "on" ? "1" : "0", sq.now(), level});
	return "Vote complete";
}

void DnDD::comment()
{
	if(id == "") content_ = "<script>alert('login first.')</script>";
	else if(stoi(level) < allow[2]) 
		content_ = "<script>alert('your level does not qualify.')</script>"; 
}

void DnDD::new_book()
{
	if(id == "") content_ = "<script>alert('login first.')</script>";
}

vector<unsigned char> base64_decode(string s);
void DnDD::edit()
{
	if(page == "0") 
		content_ = "<script>alert('This page cannot be edited.');</script>";
	else if(stoi(level) < allow[1]) 
		content_ = "<script>alert('your level does not qualify')</script>";
	else if(id == tmp["email"].asString()) {
		swap("TITLE", tmp["title"].asString());
		string s = tmp["contents"].asString();
		if(s.substr(0, 15) == "data:text/html;") {
			auto v = base64_decode(s.substr(22));
			s = string{v.begin(), v.end()};
		}
		swap("CONTENT", s);
	} else content_ = "<script>alert('you do not own this page');</script>";
}

void DnDD::index()
{
	ifstream f("carousel.txt");
	int n; string s; vector<string> v[3];
	f >> n; getline(f, s);
	for(int i=0; i<n; i++) for(int j=0; j<3; j++) {
		getline(f, s);
		v[j].push_back(s);
	}
	swap("CAROUSEL", carousel(v[0], v[1], v[2]));
}

string DnDD::search(string s)
{//return search result as a boot strap panel string
	vector<string> v1 = tables();
	string t;
	for(string table : v1) {
		sq.select( "(select tt.num, tt.page, email, title, contents, date, edit from "
				+ table + " tt inner join (select num, page, max(edit) as maxedit from "
				+ table + " group by num, page) tmp on tt.num = tmp.num and tt.page = tmp.page and tt.edit = tmp.maxedit) tt2",
				"where title like '%" + s + "%' and title <> '코멘트임.';");
		for(const auto &a : sq) {
			string n = a["num"].asString();
			string p = a["page"].asString();
			t += "<div class='panel-body'><a href='page.html?table=" + table;
			t += "&book=" + n + "&page=" + p + "'>" + table + ' ' + n;
			t += '.' + p + ". " + a["title"].asString() + "</a></div>\n";
		}
	}
	return t;
}

void DnDD::mn()
{//main.html
	if(nameNvalue_["db"] != "") {//if first connection -> set database
		sq.connect("localhost", "dndd", "dndddndd", nameNvalue_["db"]);
		if(nameNvalue_["db"] != db) db = nameNvalue_["db"], id = name = "", level="0";
		ifstream f(db + ".txt");
		getline(f, group);
		getline(f, group_desc);
		getline(f, logo);
	}
	
	vector<string> v = tables();//navbar setting
	string t;
	for(auto s : v) t += "<li><a href='main.html?field=" +s+ "'>" +s+ "</a></li>"; 
	swap("NAVITEM", t); t = "";
	table = nameNvalue_["field"] == "" ? v[0] : nameNvalue_["field"];
	swap("PANEL", field(table));
	swap("GROUP", group);
	swap("GROUP_DESC", group_desc);
	swap("LOGO", logo);
	
	if(nameNvalue_["email"] != "") {//if login attempt
		sq.select("Users", "where email = '" + nameNvalue_["email"] + "' order by date desc limit 1");
		if(sq[0]["password"] == sq.encrypt(nameNvalue_["pwd"])) {
			id = sq[0]["email"].asString();
			level = sq[0]["level"].asString();
			name = sq[0]["name"].asString();
		}
	}

	regex e{R"(<form[\s\S]+?</form>)"};
	if(id != "") content_ = regex_replace(content_, e, name + "님 레벨" + level +"으로 로그인되었습니다.");
}

void DnDD::signin()
{//sq.select returns row count
	swap("GROUP", group);
	swap("GROUP_DESC", group_desc);
	swap("LOGO", logo);
	if(nameNvalue_.empty()) return;
	if(nameNvalue_["password"] != nameNvalue_["verify"])
		append("REPLACE\">", "password not match");
	else if(sq.select("Users", "where email='" + nameNvalue_["email"] + "';"))
		append("REPLACE\">", "아이디가 이미 존재합니다.");
	else {//select will retrieve table structure, which makes inserting possible
		sq.insert({nameNvalue_["email"], sq.encrypt(nameNvalue_["password"]), "1", nameNvalue_["username"], nameNvalue_["tel"], sq.now()});
		if(nameNvalue_["check"] != "") 
			id = nameNvalue_["email"], level = "1", name = nameNvalue_["username"];
		append("REPLACE\">", "가입완료<br><a href='main.html'>메인화면으로</a><br>");
	}

	const char *append_str[]//to remember user input
		= {"email", "password", "verify", "username", "address", "tel"};
	for(string s : append_str) 
		append("id=\"" + s + '\"', " value='" + nameNvalue_[s] + '\'');
	cout << id << endl;
}

