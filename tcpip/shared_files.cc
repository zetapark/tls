#include<numeric>
#include<fcntl.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/shm.h>
#include<sys/stat.h>
#include<random>
#include"shared_files.h"
using namespace std;

void SharedMem::load(map<string, string> &&fh)
{
	uniform_int_distribution<> di{0x20, 0x72};
	random_device rd;
	for(int i=0; i<10; i++) name_[i] = di(rd);
	name_[10] = '\0';

	int offset = 0;
	for(const auto &[filename, content] : fh) {
		fileNoffset_[filename] = {offset, content.size()};
		offset += content.size();
	}
	size_ = offset;
	shm_fd_ = shm_open(name_, O_CREAT | O_RDWR, 0666);
	ftruncate(shm_fd_, size_);
	char *ptr = ptr_ = (char*)mmap(0, size_, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0);
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

void SharedMem::destroy()
{
	munmap(ptr_, size_);
	close(shm_fd_);
	shm_unlink(name_);
}
