#include<vector>
#include"bootstrap.h"
using namespace std;

BootStrapSite::BootStrapSite(string dir)
{
	WebSite::init(dir);
}

string BootStrapSite::carousel(vector<string> img, vector<string> desc, vector<string> href)
{//if you want more than 1 carousel in a page, you should change the id
	string r = R"(
<div id="myCarousel" class="carousel slide" data-ride="carousel">
  
  <!-- Indicators -->
  <ol class="carousel-indicators">
    <li data-target="#myCarousel" data-slide-to="0" class="active"></li>
	)";
	for(int i=1; i<img.size(); i++) 
		r += "<li data-target=\"#myCarousel\" data-slide-to=\"" + to_string(i) + "\"></li>\n";
	r += R"(
  </ol>
  
  <!-- Wrapper for slides -->
  <div class="carousel-inner">)";
	for(int i=0; i<img.size(); i++) {
		r += "\n\t<div class=\"item" + string(i ? "" : " active") + "\">\n";
		r += "\t\t<a href=\"" + href[i] + "\"><img class=\"center-block\" src=\"" + img[i] + "\"></a>\n";
		r += "\t\t<div class=\"carousel-caption\"><h2>" + desc[i] + "</h2></div>\n";
		r += "\t</div>\n";
	}
  	r += R"(
  </div>
  
  <!-- Left and right controls -->
  <a class="left carousel-control" href="#myCarousel" data-slide="prev">
    <span class="glyphicon glyphicon-chevron-left"></span>
    <span class="sr-only">Previous</span>
  </a>
  <a class="right carousel-control" href="#myCarousel" data-slide="next">
    <span class="glyphicon glyphicon-chevron-right"></span>
    <span class="sr-only">Next</span>
  </a>
</div>
)";
	return r;
}
