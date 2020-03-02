#include<vector>
#include<string>
#include<map>
#include<istream>

void serialize(const std::vector<std::string>& v, std::string filename);
std::vector<std::string> deserialize(std::string filename);
std::string psstm(std::string command);
std::string get_url(std::string);
std::string param(const std::string& post_string, const std::string& parameter);
std::map<std::string, int> getdir(std::string dir);
