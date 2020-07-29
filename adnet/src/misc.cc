#include<iostream>
#include"misc.h"
using namespace std;

string mailx(string src, string dst, string title, string content)
{
	string cmd = "mailx " + dst + " -r " + src + " -s '" + title + "' <<HERE_CONTENT\n"
		+ content + "\nHERE_CONTENT";
	system(cmd.data());
	return "mail sent";
}

void Misc::process() {
	if(requested_document_ == "leave_message") {
		cout << nameNvalue_["email"] << endl;
		if(nameNvalue_["dest"] == "") nameNvalue_["dest"] = "zeta@zeta2374.com";
		string cmd = "mailx " + nameNvalue_["dest"] + " -r " + nameNvalue_["email"] + " -s '" +
			nameNvalue_["title"] + "' <<HERE_CONTENT\n" + nameNvalue_["content"] + "\nHERE_CONTENT";
		system(cmd.data());
		content_ = "mail sent";
	} 
}

