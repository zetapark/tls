#include<regex>
#include<tcpip/server.h>
#include"biz.h"
#include"cvmatrix.h"//cvmatrix.h should come earlier than ocr.hpp
#include<database/util.h>//psstm
#include<text/ocr.hpp>
#include"sha256.h"
using namespace std;

void Biz::process() 
{
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "opencv.html") opencv();
	else if(requested_document_ == "insert.html") insert_bcard();
	else if(requested_document_ == "crop.html") crop();
	else if(requested_document_ == "front.jpg") content_ = front_img_;
	else if(requested_document_ == "back.jpg") content_ = back_img_;
	else if(requested_document_ == "google_oauth") google_oauth();
	else if(requested_document_ == "logout") id_ = "";
}

static string find_continuous_digits(string s, int cont, int yield=2) 
{//yield 1 or 2 non digit
	string r, y;
	bool record = false;
	for(char c : s) {
		if(isdigit(c)) {
			r += (r == "" ? "" : y) + c;
			y = "";
		} else y += c;
		if(y.size() >= yield) {
			if(r.size() > cont) return r;
			y = "";
			r = "";
		}
	}
	return r.size() > cont ? r : "";
}

static array<string, 3> get_tel(string &s)
{//return mobile, tel, fax and remove that string from s
	array<string, 3> r, tmp;
	for(int k=0,j=0; j<3; j++) {
		string tel = find_continuous_digits(s, 10, 2);
		if(tel == "") break;
		int pos = s.find(tel);
		for(int i=1; i<=7; i++) {
			bool dft = false;
			switch(toupper(s[pos - i])) {
				case 'M': r[0] = tel; break;
				case 'T': r[1] = tel; break;
				case 'F': r[2] = tel; break;
				default: dft = true;
			}
			if(!dft) {//remove tel part
				s.erase(pos - i, i + tel.size());
				break;
			} else if(i == 7) {
				tmp[k++] = tel;//if no MFT found
				s.erase(pos, tel.size());
			}
		}
	}
	for(int k=0,i=0; i<3; i++) if(r[i] == "") r[i] = tmp[k++];
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
	v.erase(it);
	return r;
}

static float wordvec(string kor, string eng, string s)
{//calculate word similarity according to language Korean or English => categorize s
	for(char &c : s) if(ispunct(c)) c = ' ';
	int english = count_if(s.begin(), s.end(), [](char c) { return isalpha(c) != 0;});
	int korean = count_if(s.begin(), s.end(), [](char c) { return isprint(c) == 0;});
	stringstream ss1; ss1 << s;
	string to_send;
	while(ss1 >> s) to_send += s + ' ' + (english < korean ? kor : eng) + ' ';
	float sum = 0;
	if(!to_send.empty()) {
		Client cl{"localhost", 3003};//connect to gensim server
		cl.send(to_send);
		cout << "sending : " << to_send << endl;
		stringstream ss;
		ss << *cl.recv();
		for(float f; ss >> f;) sum += f;
	}
	return sum;
}

static string get_with_wordvec(string kor, string eng, vector<string> &v)
{
	auto it = max_element(v.begin(), v.end(), [kor, eng](string a, string b) { return 
			wordvec(kor, eng, a) < wordvec(kor, eng, b); });
	string r = *it;
	v.erase(it);
	return r;
}

static CVMat M;

void Biz::crop() {
	vector<uint8_t> v{nameNvalue_["file"].begin(), nameNvalue_["file"].end()};
	cv::Mat mat{v, true};
	M = CVMat{cv::imdecode(mat, 1)};
	auto vp = M.get_points(4);
	swap("@IMG", base64_encode(v));

	string s;
	for(auto a : vp) s += to_string(a.x) + ',' + to_string(a.y) + ',';
	s.pop_back();
	swap("@POS", s);
	swap("@WIDTH", to_string(M.cols));
	if(s = nameNvalue_["submit"]; s == "명함 앞면 업로드") swap("@SUBMIT", "앞면 자르기");
	else if(s == "명함 뒷면 업로드") swap("@SUBMIT", "뒷면 자르기");
	append("id='name'", " value='" + nameNvalue_["name"] + "' ");//attach name if back picture
//	while(swap("@HEIGHT", to_string(M.rows)));
}

static void get_b(string s)
{
	stringstream ss; ss << s;
	char c; float x, y;
	vector<cv::Point> vp;
	for(int i=0; i<8; i+=2) {
		ss >> c >> x >> c >> y;
		vp.push_back({x, y});
	}
	M.get_businesscard(vp);
}

void Biz::opencv()
{
	if(string s = nameNvalue_["submit"]; s != "") {
		get_b(nameNvalue_["pos"]);
		vector<uint8_t> v;
		cv::imencode(".jpg", M, v);
		namecard_ = base64_encode(v);
		if(s == "앞면 자르기") front_img_ = string{v.begin(), v.end()};
	}

	if(nameNvalue_["submit"] == "뒷면 자르기") {//from crop && name != "" back image upload
		int num = 1;
		sq.query("select max(num) from image where user = '" + id_ + "';");
		if(sq.fetch(1)) num += sq[0][""].asInt();
		sq.query("update bcard set back=" + to_string(num) + " where user='" + 
				id_ + "' and name='" + nameNvalue_["name"] + "'");
		sq.select("image", "limit 1");
		sq.insert(id_, num, namecard_);
	} else if(nameNvalue_["submit"] == "앞면 자르기") {
		auto a = cv::text::OCRTesseract::create(NULL, "eng+kor");
		string s, email;
		vector<cv::Rect> vr; vector<string> vs; vector<float> vf;
		a->run(M, s, &vr, &vs, &vf); vs.clear();
		swap("@DATA", s);
		if(smatch sm; regex_search(s, sm, regex{R"(\S+@\S+\.\S+)"})) {
			email = sm[0].str();
			s = sm.prefix().str() + sm.suffix().str();
		}
		auto tel = get_tel(s);
		stringstream ss; ss << s;
		while(getline(ss, s)) if(s != "") vs.push_back(s);
		auto it = remove_if(vs.begin(), vs.end(), [](string s) { return 
				find_if(s.begin(), s.end(), [](char c) { return !isspace(c);}) == s.end(); });
		vs.erase(it, vs.end());//remove blank string

		append("name='email'", " value='" + email + "' ");
		append("name='mobile'", " value='" + tel[0] + "' ");
		append("name='tel'", " value='" + tel[1] + "' ");
		append("name='fax'", " value='" + tel[2] + "' ");
		if(!vs.empty())
			append("name='role'", " value='" + get_with_wordvec("직책", "job", vs) + "' ");
		if(!vs.empty())
			append("name='job'", " value='" + get_with_wordvec("직장", "company", vs) + "' ");
		if(!vs.empty()) append("name='addr1'", " value='" + get_addr(vs) + "' ");
		if(!vs.empty())
			append("name='addr2'", " value='" + get_with_wordvec("주소", "address", vs) + "' ");
		if(!vs.empty()) append("name='name'", " value='" + get_name(vs) + "' ");
		for(int i=1; i<=vs.size() && i<=3; i++)
			append("name='memo" + to_string(i) + "'", " value='" + vs[i-1] + "' ");
	}

	if(nameNvalue_["name"] != "") {
		swap("false", "true");//readonly setting
		sq.select("bcard", "where user = '" + id_ + "' and name = '" + nameNvalue_["name"] + "'");
		append("name='email'", " value='" + sq[0]["email"].asString() + "' ");
		append("name='mobile'", " value='" + sq[0]["mobile"].asString() + "' ");
		append("name='tel'", " value='" + sq[0]["tel"].asString() + "' ");
		append("name='fax'", " value='" + sq[0]["fax"].asString() + "' ");
		append("name='role'", " value='" + sq[0]["role"].asString() + "' ");
		append("name='job'", " value='" + sq[0]["company"].asString() + "' ");
		append("name='email'", " value='" + sq[0]["email"].asString() + "' ");
		append("name='addr1'", " value='" + sq[0]["address1"].asString() + "' ");
		append("name='addr2'", " value='" + sq[0]["address2"].asString() + "' ");
		append("name='name'", " value='" + sq[0]["name"].asString() + "' ");
		append("name='memo1'", " value='" + sq[0]["memo1"].asString() + "' ");
		append("name='memo2'", " value='" + sq[0]["memo2"].asString() + "' ");
		append("name='memo3'", " value='" + sq[0]["memo3"].asString() + "' ");

		int back_num = sq[0]["back"].asInt(), front_num = sq[0]["front"].asInt();
		if(!front_num) front_img_ = "";
		else if(sq.select("image", "where user='" + id_ + "' and num=" + to_string(front_num))) {
			auto v = base64_decode(sq[0]["image"].asString());
			front_img_ = string{v.begin(), v.end()};
		}
		if(!back_num) back_img_ = "";
		else if(sq.select("image", "where user='" + id_ + "' and num=" + to_string(back_num))) {
			auto v = base64_decode(sq[0]["image"].asString());
			back_img_ = string{v.begin(), v.end()};
		}
	}
}

void Biz::insert_bcard()
{
	if(id_ == "") return;
	int num = 1, back_num = 0;
	sq.query("select max(num) from image where user = '" + id_ + "';");
	if(sq.fetch(1)) {
		cout << "sq : " << sq << endl;
		num += sq[0][""].asInt();
	}
	sq.select("bcard", "where user='" + id_ + "' and name='" + nameNvalue_["hidden"] + "'");
	if(nameNvalue_["submit"] == "수정완료") {
		num = sq[0]["front"].asInt();
		back_num = sq[0]["back"].asInt();
		sq.query("delete from bcard where user='" + id_ + "' and name='" +
				nameNvalue_["hidden"] + "'");
	}
	cout << sq << endl;
	sq.insert(id_, nameNvalue_["name"], nameNvalue_["addr1"], nameNvalue_["addr2"],
			nameNvalue_["job"], nameNvalue_["role"], nameNvalue_["mobile"], 
			nameNvalue_["tel"], nameNvalue_["fax"], nameNvalue_["email"], 
			nameNvalue_["memo1"], nameNvalue_["memo2"], nameNvalue_["memo3"], num, back_num);
	if(nameNvalue_["submit"] == "제출") {
		sq.select("image", "limit 1");
		sq.insert(id_, num, namecard_);
	}
}

void Biz::google_oauth()
{
	if(string s = psstm("./oauth.py " + nameNvalue_["token"]); s != "") {//s == email
		s.pop_back();//\n remove
		if(!sq.select("user", "where email='" + s + "'")) sq.insert(s, "");
		if(id_ == "") content_ = "reload";//in case login first
		id_ = s;
	}
}

void Biz::db_ip(string ip) { db_ip_ = ip; }
void Biz::index()
{
	M.clear();
	sq.connect(db_ip_, "bcard", "bcard", "businesscard");
	string enc;
	if(string s = nameNvalue_["pwd"]; s != "") {
		SHA2 sha;
		auto a = sha.hash(s.begin(), s.end());
		enc = base64_encode({a.begin(), a.end()});
	}
	if(string s = nameNvalue_["name"]; id_ != "" && s != "")//if from delete button
			sq.query("delete from bcard where user='" + id_ + "' and name='" + s + "'");
	if(nameNvalue_["submit"] == "login") {
		if(sq.select("user", "where email = '" + nameNvalue_["email"]
					+"' and password = '" + enc + "'"))
			id_ = nameNvalue_["email"];
	} else if(nameNvalue_["submit"] == "signin") {
		if(!sq.select("user", "where email ='" + nameNvalue_["email"] + "'")) {
			sq.insert(nameNvalue_["email"], enc);
			id_ = nameNvalue_["email"];
		}
	}

	if(id_ != "") {//if already logged
		swap("hidden", "visible");//show logout button
		regex e{R"(<div id=login[\s\S]+?</div>\s+</div>)"};
		content_ = regex_replace(content_, e, id_ + "님 로그인되었습니다.",
				regex_constants::format_first_only);
		sq.select("bcard", "where user = '" + id_ + "' order by name");
		string s;
		for(auto a : sq)
			s += "<a href='opencv.html?name=" + a["name"].asString() + "'>" + 
				a["name"].asString() + ", " + a["company"].asString() + "</a><br>";
		prepend("</body>", s);
	}
}

