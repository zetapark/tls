#include<fstream>
#include<sstream>
#include<cstring>
#include<iostream>
#include<experimental/filesystem>
#include<regex>
#include<csignal>
#include"server.h"
#include"website.h"
using namespace std;
using namespace std::experimental::filesystem;

SharedMem WebSite::fileNhtml_;
multimap<string, string> WebSite::added_header_;

void destroy_shared_mem(int sig)
{
	WebSite::fileNhtml_.destroy();
	cout << "destroyed shared map, exiting " << sig << endl;
	exit(sig);
}

void WebSite::init(string dir)
{
	map<string, string> fnh;
	if(dir.back() == '/') dir.pop_back();
	for(const path& a : recursive_directory_iterator{dir}) {//directory entry has operator path
		if(!is_directory(a)) {
			ifstream f(a.string()); string s; char c;
			while(f >> noskipws >> c) s += c;
			fnh[a.relative_path().string().substr(dir.size() + 1)] = s;
			cout << "loading " << a << endl;
		}
	}
	fileNhtml_.load(move(fnh));
	signal(SIGINT, destroy_shared_mem);
}

bool WebSite::swap(string b, string a)
{//child classes will use this to change content_
	if(content_.find(b) == string::npos) return false;
	content_.replace(content_.find(b), b.size(), a);
	return true;	
}

bool WebSite::append(string a, string b)
{
	if(content_.find(a) == string::npos) return false;
	content_.insert(content_.find(a) + a.size(), b);
	return true;	
}

bool WebSite::prepend(string a, string b)
{
	if(content_.find(a) == string::npos) return false;
	content_.insert(content_.find(a), b);
	return true;
}

void WebSite::add_header(string requested_document, string header) 
{//Access-Control-Allow-Origin: https://foo.example
	added_header_.insert({requested_document, header});
}

std::string WebSite::operator()(string s) 
{//will set requested_document and nameNvalue (= parameter of post or get)
	parse_all(move(s));
	try {
		process();//derived class should implement this -> set content_ & cookie
	} catch(const exception& e) {
		cerr << e.what() << endl;
	}
	return return_content();
}

void WebSite::parse_all(string &&s)
{
	nameNvalue_.clear();
	stringstream ss; ss << s; ss >> s;
	if(s == "POST") {//parse request and header
		ss >> requested_document_;
		requested_document_ = requested_document_.substr(1);
		string boundary;
		while(s != "\r") {//check \r\n[\r]\n
			getline(ss, s);
			if(s.find("Content-Type: multipart/form-data;") == 0) {
				boundary = s.substr(s.find("boundary=") + 9);
				boundary.pop_back();
			} else if(auto pos = s.find(": "); pos != string::npos)
				req_header_[s.substr(0, pos)] = s.substr(pos + 2, s.size() - pos - 3);
		}
		if(boundary == "") nameNvalue_ = parse_post(ss);
		else parse_multi(ss, boundary);
	} else if(s == "GET") {
		ss >> s;
		stringstream ss2; ss2 << s;//GET '/login.html?adf=fdsa'
		getline(ss2, s, '?');
		requested_document_ = s.substr(1);//get rid of '/'
		nameNvalue_ = parse_post(ss2);

		while(getline(ss, s)) if(auto pos = s.find(": "); pos != string::npos)
			req_header_[s.substr(0, pos)] = s.substr(pos + 2, s.size() - pos - 3);
	}
	if(requested_document_ == "") requested_document_ = "index.html";
	content_ = fileNhtml_[requested_document_];
	cout << "requested_document : " << requested_document_ << endl;
}

string WebSite::return_content() 
{
	string s;
	const auto &[begin, end] = added_header_.equal_range(requested_document_);
	for(auto it = begin; it != end; it++) s += it->second + "\r\n";
	return header() + s + "Content-Length: " + to_string(content_.size()) + "\r\n\r\n" + content_;
}

string WebSite::header()
{
	const char *extension[][2] = {
		{".aac", "audio/aac"},
		{".abw", "application/x-abiword"},
		{".arc", "application/x-freearc"},
		{".avi", "video/x-msvideo"},
		{".azw", "application/vnd.amazon.ebook"},
		{".bin", "application/octet-stream"},
		{".bmp", "image/bmp"},
		{".bz", "application/x-bzip"},
		{".bz2", "application/x-bzip2"},
		{".csh", "application/x-csh"},
		{".css", "text/css"},
		{".csv", "text/csv"},
		{".doc", "application/msword"},
		{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
		{".eot", "application/vnd.ms-fontobject"},
		{".epub", "application/epub+zip"},
		{".gz", "application/gzip"},
		{".gif", "image/gif"},
		{".htm", "text/html"},
		{".html", "text/html"},
		{".ico", "image/vnd.microsoft.icon"},
		{".ics", "text/calendar"},
		{".jar", "application/java-archive"},
		{".jpeg", "image/jpeg"},
		{".jpg", "image/jpeg"},
		{".js", "text/javascript"},
		{".json", "application/json"},
		{".jsonld", "application/ld+json"},
		{".mid", "audio/midi"},
		{".midi", "audio/midi"},
		{".mjs", "text/javascript"},
		{".mp3", "audio/mpeg"},
		{".mpeg", "video/mpeg"},
		{".mpkg", "application/vnd.apple.installer+xml"},
		{".odp", "application/vnd.oasis.opendocument.presentation"},
		{".ods", "application/vnd.oasis.opendocument.spreadsheet"},
		{".odt", "application/vnd.oasis.opendocument.text"},
		{".oga", "audio/ogg"},
		{".ogv", "video/ogg"},
		{".ogx", "application/ogg"},
		{".opus", "audio/opus"},
		{".otf", "font/otf"},
		{".png", "image/png"},
		{".pdf", "application/pdf"},
		{".php", "application/x-httpd-php"},
		{".ppt", "application/vnd.ms-powerpoint"},
		{".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
		{".rar", "application/vnd.rar"},
		{".rtf", "application/rtf"},
		{".sh", "application/x-sh"},
		{".svg", "image/svg+xml"},
		{".swf", "application/x-shockwave-flash"},
		{".tar", "application/x-tar"},
		{".tif", "image/tiff"},
		{".tiff", "image/tiff"},
		{".ts", "video/mp2t"},
		{".ttf", "font/ttf"},
		{".txt", "text/plain"},
		{".vsd", "application/vnd.visio"},
		{".wav", "audio/wav"},
		{".weba", "audio/webm"},
		{".webm", "video/webm"},
		{".webp", "image/webp"},
		{".woff", "font/woff"},
		{".woff2", "font/woff2"},
		{".xhtml", "application/xhtml+xml"},
		{".xls", "application/vnd.ms-excel"},
		{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
		{".xml", "text/xml"},
		{".xul", "application/vnd.mozilla.xul+xml"},
		{".zip", "application/zip"},
		{".3gp", "video/3gpp"},
		{".3g2", "video/3gpp2"},
		{".7z", "application/x-7z-compressed"}
	};

	string r1 = "HTTP/1.1 200 OK\r\nContent-Type: ", r2 = "; charset=utf-8\r\n";
	for(const auto &p : extension)
		if(requested_document_.substr(requested_document_.rfind('.')) == p[0]) 
			return r1 + p[1] + r2;
	return r1 + "text/html" + r2;
}

istream& WebSite::parse_one(istream& is, string boundary)
{
	regex e1{R"raw(name="(\w+)")raw"}, e2{R"raw(filename="(\S+)")raw"};
	smatch m; string s, name, filename, val;
	if(!getline(is, s)) return is;
	if(regex_search(s, m, e1)) name = m[1].str();
	s = m.suffix().str();
	if(regex_search(s, m, e2)) filename = m[1].str();
	do getline(is, s);//skip
	while(s != "\r");
	
	while(getline(is, s)) {//parse value
		if(s.find(boundary) != string::npos) break;
		val += s + '\n';
	}
	val.pop_back();// + \n
	val.pop_back();//\r
	nameNvalue_[name] = val;
	if(filename != "") nameNvalue_[name + "filename"] = filename;
	return is;
}

void WebSite::parse_multi(istream& is, string boundary)
{
	string s;
	getline(is, s);
	while(parse_one(is, boundary));
}

map<string, string> WebSite::parse_post(istream& post)
{
	map<string, string> m;
	string s, value;
	while(getline(post, s, '&')) {
		cout << s << endl;
		int pos = s.find('=');
		if(s.back() == '=') value = "";
		else {
			value = s.substr(pos+1);
			for(auto& a : value) if(a == '+') a = ' ';
			for(int i = value.find('%'); i != string::npos; i = value.find('%', i))
				value.replace(i, 3, 1, (char)stoi(value.substr(i + 1, 2), nullptr,16));
			if(value.back() == '\0') value.pop_back();
		}
		m[s.substr(0, pos)] = value;
		cout << value << endl;
	}
	return m;
}

string carousel(vector<string> img, vector<string> desc, vector<string> href)
{//if you want more than 1 carousel in a page, you should change the id
	string r = R"(
<div id="myCarousel" class="carousel slide" data-ride="carousel">
  
  <!-- Indicators -->
  <ol class="carousel-indicators">
    <li data-target="#myCarousel" data-slide-to="0" class="active"></li>
	)";
	for(int i=1; i<img.size(); i++) 
		r += "<li data-target=\"#myCarousel\" data-slide-to=\"" + to_string(i) + "\"></li>\n";
	r += R"(
  </ol>
  
  <!-- Wrapper for slides -->
  <div class="carousel-inner">)";
	for(int i=0; i<img.size(); i++) {
		r += "\n\t<div class=\"item" + string(i ? "" : " active") + "\">\n";
		r += "\t\t<a href=\"" + href[i] + "\"><img class=\"center-block\" src=\"" + img[i] + "\"></a>\n";
		r += "\t\t<div class=\"carousel-caption\"><h2>" + desc[i] + "</h2></div>\n";
		r += "\t</div>\n";
	}
  	r += R"(
  </div>
  
  <!-- Left and right controls -->
  <a class="left carousel-control" href="#myCarousel" data-slide="prev">
    <span class="glyphicon glyphicon-chevron-left"></span>
    <span class="sr-only">Previous</span>
  </a>
  <a class="right carousel-control" href="#myCarousel" data-slide="next">
    <span class="glyphicon glyphicon-chevron-right"></span>
    <span class="sr-only">Next</span>
  </a>
</div>
)";
	return r;
}
