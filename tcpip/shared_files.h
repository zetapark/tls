#include<map>
#include<string>
#include<utility>

class SharedMem
{
public:
	void load(std::map<std::string, std::string> &&fileNhtml);
	~SharedMem();
	std::string operator[](std::string filename);

protected:
	std::map<std::string, std::pair<int, int>> fileNoffset_;
	char *ptr_;

private:
	const char *name_ = "fileNhtml";
};
