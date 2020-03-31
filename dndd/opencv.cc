#include<regex>
#include"cvmatrix.h"//cvmatrix.h should come earlier than ocr.hpp
#include<opencv2/text/ocr.hpp>
#include"tcpip/server.h"
#include"dndd.h"
#include"src/cert_util.h"
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
		for(int i=1; i<=5; i++) {
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
			} else if(i == 5) {
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
	cout << "이름은" << r <<  "." << endl;
	v.erase(it);
	return r;
}

static float wordvec(string kor, string eng, string s)
{
	string to_send;
	int english = count_if(s.begin(), s.end(), [](char c) { return isalpha(c) != 0;});
	int korean = count_if(s.begin(), s.end(), [](char c) { return isprint(c) == 0;});
	stringstream ss1; ss1 << s;
	while(ss1 >> s) {
	//	s.erase(remove_if(s.begin(), s.end(), [](char c) { return ispunct(c) != 0;}), s.end());
		if(!s.empty()) to_send += s + ' ' + (english < korean ? kor : eng) + ' ';
	}
	Client cl{"localhost", 3003};
	if(!to_send.empty()) cl.send(to_send);
	stringstream ss;
	ss << *cl.recv();
	vector<float> vf;
	for(float f; ss >> f;) vf.push_back(f);
	vf.erase(std::remove(vf.begin(), vf.end(), 0), vf.end());
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
	smatch sm;
	regex_search(s, sm, regex{R"(\S+@\S+\.\S+)"});
	string email = sm[0].str();
	s = sm.prefix().str() + sm.suffix().str();
	auto tel = get_tel(s);
	vs.clear();
	stringstream ss; ss << s;
	while(getline(ss, s)) if(s != "") vs.push_back(s);
	auto it = remove_if(vs.begin(), vs.end(), [](string s) { return 
			find_if(s.begin(), s.end(), [](char c) { return !isspace(c);}) == s.end(); });
	vs.erase(it, vs.end());//remove blank string
	
	append("name='email'", " value='" + email + "' ");
	append("name='mobile'", " value='" + tel[0] + "' ");
	append("name='tel'", " value='" + tel[1] + "' ");
	append("name='fax'", " value='" + tel[2] + "' ");
	append("name='job'", " value='" + get_with_wordvec("직장", "company", vs) + "' ");
	append("name='role'", " value='" + get_with_wordvec("직책", "role", vs) + "' ");
	append("name='addr1'", " value='" + get_addr(vs) + "' ");
	append("name='addr2'", " value='" + get_with_wordvec("주소", "address", vs) + "' ");
	append("name='name'", " value='" + get_name(vs) + "' ");
	for(int i=1; i<=vs.size() && i<=3; i++)
		append("name='memo" + to_string(i) + "'", " value='" + vs[i-1] + "' ");

//	for(int i=0; i<vr.size(); i++) {
//		imshow(vs[i] + to_string(vf[i]), t(vr[i]));
//		cout << vr[i] <<' '<< vf[i] <<' '<< vs[i] << endl;
//	}

	cv::imencode(".jpg", m, v);
	namecard_ = base64_encode(v);
	swap("@IMG2", namecard_);
}

