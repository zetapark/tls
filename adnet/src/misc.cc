#include"misc.h"
using namespace std;

void Misc::process(sockaddr_in&&) {
	if(requested_document_ == "leave_message") {
		if(nameNvalue_["dest"] == "") nameNvalue_["dest"] = "zeta@zeta2374.com";
		string cmd = "mailx " + nameNvalue_["dest"] + " -r " + nameNvalue_["email"] + " -s '" +
			nameNvalue_["title"] + "' <<HERE_CONTENT\n" + nameNvalue_["content"] + "\nHERE_CONTENT";
		system(cmd.data());
		content_ = "mail sent";
	} 
}

