#include"misc.h"
using namespace std;

void Misc::process() {
	if(requested_document_ == "leave_message") {
		string cmd = "mailx zeta@zeta2374.com -r " + nameNvalue_["email"] + " -s '" +
			nameNvalue_["title"] + "' <<HERE_CONTENT\n" + nameNvalue_["content"] + "\nHERE_CONTENT";
		system(cmd.data());
		content_ = "mail sent";
	} 
}

