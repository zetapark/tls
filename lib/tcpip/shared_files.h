#pragma once
#include<map>
#include<string>
#include<utility>

class SharedMem
{
public:
	void load(std::map<std::string, std::string> &&fileNhtml);
	void destroy();
	std::string operator[](std::string filename);

protected:
	std::map<std::string, std::pair<int, int>> fileNoffset_;
	char *ptr_;
	int shm_fd_, size_;

private:
	char name_[11];
};
