#include<numeric>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include"shared_files.h"
using namespace std;

void SharedMem::load(map<string, string> &&fh)
{
	int offset = 0;
	for(const auto &[filename, content] : fh) {
		fileNoffset_[filename] = {offset, content.size()};
		offset += content.size();
	}
	int shm_fd = shm_open(name_, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd, offset);
	char *ptr = (char*)mmap(0, offset, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	ptr_ = ptr;
	for(const auto &[filename, content] : fh) {
		std::copy(content.cbegin(), content.cend(), ptr);
		ptr += content.size();
	}
}

string SharedMem::operator[](string s)
{
	auto a = fileNoffset_[s];
	return {ptr_ + a.first, ptr_ + a.first + a.second};
}

SharedMem::~SharedMem()
{
	shm_unlink(name_);
}
