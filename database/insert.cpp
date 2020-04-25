#include<regex>
#include<string>
#include<fstream>
#include"mysqldata.h"
using namespace std;

int main(int ac, char **av)
{
	SqlQuery sq;
	sq.connect("localhost", "zeta", "cockcodk0", "kyungki");
	regex e{R"(<tr>\s+<td>(.+?)</td>\s+<td>(.+?)</td>\s+<td>(.+?)</td>\s+<td>(.+?)</td>\s+</tr>)"};
	for(int i=1; i<ac; i++) {
		string s;
		ifstream f{av[i]};
		for(char c; f >> noskipws >> c;) s += c;
		smatch m;
		sq.select("가맹점", "limit 1");
		while(regex_search(s, m ,e)) {
			sq.insert(m[1].str(), m[2].str(), m[3].str(), m[4].str());
			s = m.suffix();
		}
	}
}

/*
from selenium import webdriver
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import os, time
import sqlalchemy as sql
from bs4 import BeautifulSoup

drv = webdriver.Firefox()
drv.get("http://gmoney.or.kr/main/gmoney/searchFranchisee.do?menuNo=040000&subMenuNo=040100")
conn = sql.create_engine('mysql://zeta:cockcodk0@localhost/kyungki?charset=utf8')

 for i in range(390, 492):
    drv.find_element_by_xpath("//a[@href='?pageIndex="+str(i+1)+"']").click();
    time.sleep(3)
    f = open('kk/' + str(i) + '.txt', mode='wt', encoding='utf-8')
    f.write(drv.page_source)
    f.close()
  */  
