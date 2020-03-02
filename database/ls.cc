#include<map>
#include<dirent.h>
#include<iostream>
using namespace std;

map<string, int> getdir(string dir)
{///return map<filename, type>
	map<string, int> files;
    DIR *dp;
    struct dirent *dirp;
	int errno;
    if((dp  = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        throw errno;
    }

    while ((dirp = readdir(dp)) != NULL) {
        files.insert({string(dirp->d_name), dirp->d_type});
    }
    closedir(dp);
	for(auto& a : files) if(a.first[0] == '.') files.erase(a.first);
    return files;//error
}


