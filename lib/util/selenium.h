#include<pybind11/pybind11.h>
#include<pybind11/embed.h>
#include"cpp_graph.h"

class Selenium 
{
public:
	Selenium(std::string phantom_path);
	~Selenium();
	void crawl(int depth);
	void save_graph();
	void title_tag(std::string s), title_class(std::string s), seller_tag(std::string s), 
			 seller_class(std::string s), output_file(std::string s), graph_file(std::string s),
			 start_url(std::string s), delay(int i);//setters
	
protected:
	pybind11::scoped_interpreter guard_;//this class should be allocated before any python module
	pybind11::object webdriver, drv, bs, json;
	std::string title_tag_, title_class_, seller_tag_, seller_class_, output_file_,
		graph_file_, start_url_, phantom_path_;
	int delay_ = 5;
	Graph<std::string> grp_;

private:
	std::string get_url(std::string url);
	std::vector<std::string> extract(std::string doc) const;
	void insert_data(std::string link);
	void crawl(int idx, int depth, bool read);
};
