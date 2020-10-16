#include<vector>
#include<fstream>
using namespace std;

string base64_encode(vector<unsigned char> v);
string content_;
bool swap(string b, string a)
{//child classes will use this to change content_
	if(content_.find(b) == string::npos) return false;
	content_.replace(content_.find(b), b.size(), a);
	return true;	
}

int main()
{
	const char *p[] = {"/home/zeta/MEGA/Project/mail2.html", "/home/zeta/Pictures/cute/1.jpg", "/home/zeta/Pictures/cute/2.jpg", "/home/zeta/Pictures/cute/3.jpg", "/home/zeta/Pictures/cute/browser_design.jpg", "/home/zeta/Pictures/cute/circle.jpg", "/home/zeta/MEGA/Project/mail3.html"};

	ifstream f[6];
	for(int i=0; i<6; i++) f[i].open(p[i]);
	ofstream ofs{p[6]};
	uint8_t u;
	string s[6];
	for(char c; f[0] >> noskipws >> c; s[0] += c);
	for(int i=1; i<=5; i++) {
		vector<uint8_t> v;
		while(f[i] >> noskipws >> u) v.push_back(u);
		s[i] = base64_encode(v);
	}
	content_ = s[0];
	for(int i=1; i<=5; i++) swap("@" + to_string(i), s[i]);
	ofs << content_;
}

