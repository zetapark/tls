#include<regex>
#include"tcpip/server.h"
#include"dndd.h"
#include"src/cert_util.h"
#include"cvmatrix.h"//cvmatrix.h should come earlier than ocr.hpp
#include<text/ocr.hpp>
using namespace std;

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

void DnDD::crop() {
	vector<uint8_t> v{nameNvalue_["file"].begin(), nameNvalue_["file"].end()};
	cv::Mat mat{v, true};
	M = CVMat{cv::imdecode(mat, 1)};
	auto vp = M.get_points(4);
	swap("@IMG", base64_encode(v));

	string s;
	for(auto a : vp) s += to_string(a.x) + ',' + to_string(a.y) + ',';
	s.pop_back();
	swap("@POS", s);
	while(swap("@WIDTH", to_string(M.cols)));
	while(swap("@HEIGHT", to_string(M.rows)));
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

void DnDD::opencv() {
	get_b(nameNvalue_["pos"]);
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

	vector<uint8_t> v;
	cv::imencode(".jpg", M, v);
	namecard_ = base64_encode(v);
	swap("@IMG2", namecard_);
}

void DnDD::insert_bcard()
{
	if(id2_ == "") return;
	int num = 1;
	if(sq2.query("select max(num) from image where user = '" + id2_ + "';")) 
		num += sq2[0][""].asInt();
	cout << sq2 << endl;
	sq2.select("bcard", "limit 1");
	sq2.insert(id2_, nameNvalue_["name"], nameNvalue_["addr1"], nameNvalue_["addr2"],
			nameNvalue_["job"], nameNvalue_["role"], nameNvalue_["mobile"], 
			nameNvalue_["tel"], nameNvalue_["fax"], nameNvalue_["email"], 
			nameNvalue_["memo1"], nameNvalue_["memo2"], nameNvalue_["memo3"], num, 0);
	sq2.select("image", "limit 1");
	sq2.insert(id2_, num, namecard_);
}

void DnDD::busi()
{
	if(nameNvalue_["email"] != "") {//if login or signin
		if(nameNvalue_["submit"] == "login") {
			if(sq2.select("user", "where email = '" + nameNvalue_["email"]
						+"' and password = '" + sq.encrypt(nameNvalue_["pwd"]) + "'"))
				id2_ = sq2[0]["email"].asString();
		} else if(nameNvalue_["submit"] == "signin") {
			if(!sq2.select("user", "where email ='" + nameNvalue_["email"] + "'")) {
				sq2.insert(nameNvalue_["email"], sq2.encrypt(nameNvalue_["pwd"]));
				id2_ = nameNvalue_["email"];
			}
		}
	}

	if(id2_ != "") {//if already logged
		regex e{R"(<form[\s\S]+?</form>)"};
		content_ = regex_replace(content_, e, id2_ + "님 로그인되었습니다.",
				regex_constants::format_first_only);
		sq2.select("bcard", "where user = '" + id2_ + "'");
		string s;
		for(auto a : sq2)
			s += "<a href='view.html?name=" + a["name"].asString() + "'>" + a["name"].asString() + "</a><br>";
		prepend("</body>", s);
	}
}

void DnDD::view()
{
	sq2.select("bcard", "where user = '" + id2_ + "' and name ='" + nameNvalue_["name"] + "'");
	append("이름 :</td><td>", sq2[0]["name"].asString());
	append("주소1 :</td><td>", sq2[0]["address1"].asString());
	append("주소2 :</td><td>", sq2[0]["address2"].asString());
	append("직장 :</td><td>", sq2[0]["company"].asString());
	append("직책 :</td><td>", sq2[0]["role"].asString());
	append("모바일 :</td><td>", sq2[0]["mobile"].asString());
	append("전화 :</td><td>", sq2[0]["tel"].asString());
	append("팩스 :</td><td>", sq2[0]["fax"].asString());
	append("이메일 :</td><td>", sq2[0]["email"].asString());
	append("메모1 :</td><td>", sq2[0]["memo1"].asString());
	append("메모2 :</td><td>", sq2[0]["memo2"].asString());
	append("메모3 :</td><td>", sq2[0]["memo3"].asString());
	sq2.select("image", "where user='" + id2_ + "' and num=" + sq2[0]["front"].asString());
	swap("@IMG", sq2[0]["image"].asString());
}
