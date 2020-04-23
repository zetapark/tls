#include"biz.h"
using namespace std;

void Biz::process() 
{
	if(requested_document_ == "index.html") index();
	else if(requested_document_ == "opencv.html") opencv();
	else if(requested_document_ == "insert.html") insert_bcard();
	else if(requested_document_ == "crop.html") crop();
	else if(requested_document_ == "front.jpg") content_ = front_img_;
	else if(requested_document_ == "back.jpg") content_ = back_img_;
}

void Biz2::process()
{}
