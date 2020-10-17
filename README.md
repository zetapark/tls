<div align=center><img src=https://tomcat.zeta2374.com/image/ez.png height=200></div>

# eZ Framework는 stateful C++ Server Side Web Development Framework입니다.






## Try your first eZ Framework

```html
<html>
<h4>It works, eZ Framework!!</h4>
</html>
```
put this file into site_html directory.
```c++
#include"tcpip/server.h"
#include"tcpip/website.h"

int main() {//web server program
	WebSite my_site;
	my_site.init("site_html");//html template directory
	Server sv{2000};//port number
	sv.start(my_site);//go infinite loop
}
```

If you access localhost port 2000 with your browser, you will see the  result.

><h4> It works, eZ Framework!!</h4>



## Branch

- master : TLS 구현으로 배우는 암호학
- opensw : 공개 SW 개발자 대회
- develop : up to date branch



## Directory Structure

- lib : eZ Framework source(subdirectories are below)

  - tcpip : for tcpip, http protocol
  - database : mysql database wrapper
  - tls : TLS 1.3, 1.2 library
  - util : utility classes
  - tst : test cases

- Made with eZ Framework
	- adnet : Banner Ad platform [AdNET](https://adnet.zeta2374.com)
	- biz : Business card organizer [Biz](https://biz.zeta2374.com)
	- dndd : Bulletin board system [Discussion and Democratic Decision making](https://dndd.zeta2374.com)
	- ez : eZ Framework official site [eZ](https://www.zeta2374.com/introduction.html)
	- suwon : stores in suwon city that use Kyungki province local currency [경기화폐](https://suwon.zeta2374.com)
	
- incltouch : makefile utility program

  

## Install

##### - get source from Github

```bash
git clone https://github.com/zetapark/tls
cd tls
git checkout opensw
```

##### - core dependency : nettle, gmp++, jsoncpp

```bash
sudo apt install libjsoncpp-dev nettle-dev libgmpxx4ldbl
```

##### - database utility etc dependency : mysqlcppconn, pybind11, catch

```bash
sudo apt install libmysqlcppconn-dev pybind11-dev catch
```

##### - business card web app dependency : opencv, gensim

```bash
sudo apt install libopencv-dev tesseract-ocr tesseract-ocr-eng tesseract-ocr-kor 
pip3 install gensim
```
##### - compile

run make at the root directory. All the files will be built.
If it fails, run make safe first. This will just compile eZ Framework core.
then, run make. or install omitted libraries.

```bash
make || make safe
```





## Features



#### - Library is to be used, not to be learned : Very easy

There are so many things to learn and memorize. We don't want to add one more burden.
eZ Framework is a very easy server side web development framework.
You just need to memorize just a few things to develop with eZ framework.
The whole reference is just about 1~2 pages. The rest depends on the developer's adaptability.
A complex business card web app could be made with this small set of functionalities.
eZ framework provides very simple web development environment.
Developer can just pretend that the program is exchanging messages with one client.
Web site is defined by a C++ class, thus enabling convenient structured programming.



#### - Virtual programming environment with State : Use full power of C++

C++ is a mature, versatile and very fast language. But it was almost never used in web development. The biggest reason is that web development is different from normal software development. It is segmented by pages users are seeing,  because HTTP protocol is stateless. This is why script-like languages are preferred in web development, rather than fully edged languages. And, we need to use cookie or some other complex methods to store informations like login status. eZ framework provides a stateful virtual environment that a browser is constantly connected to a web server. This virtual environment removes the segmentation of web apps. Thanks to this, a programmer can just store those informations, that should be sustained through multiple pages, to a normal C++ variables. This is very convenient. No more queries or cookies are needed to verify if the user is logged in. Just look at the local variable. 

In eZ Framwork, You will define your website as a class. This will make structured programming easy. You can roam free in your server programming as you are programming a normal application.



#### - Middle Server

<img src=https://www.zeta2374.com/middleserver.png width=480>

eZ framework directly makes an executable to run as a web server. We do not need to run behemothian Apache or tomcat server. eZ framework is composed of a middle server and multiple web servers. Middle server enables https encryption and distributing connections to web servers. This middle server is very flexible that it can provide a https connection to an apache server without any configuration change. Try this link to see how simple it is.[https://tomcat.zeta2374.com/tomcat.html](Middle server is providing Encrypted Connection to Apache Tomcat.) Of course, if it is located between web server made with eZ framework and browser, it can provide a stateful virtual environmet.

Middle server categorize packets by browsers. Middle server makes some proxy client inside of itself to connect to a web server continuously. Middle server uses three methods to identify the packets according to browser.

- TLS 1.3 session resumption ticket

- TLS 1.2 session ID

- Cookie

  

#### - Easy Template Usage

eZ framework can use HTML, CSS, Java Script as template. But eZ framework does not have complex template grammar. We just provide swap, append, prepend functions. If you are familiar with regular expressions, you can use it. Template is not different from static web page. You can think template usage like this.

- Make a static web page
- Transform it in server side if needed
- Send it to browser
```html
<html>
<h4>It works, eZ Framework!!</h4>
</html>
```
```C++
#include"tcpip/server.h"
#include"tcpip/website.h"

struct My_site : WebSite {//my website definition
    void process() {//server side programming
        append("It", " really");
    }
};

int main() {
	My_site my_site;
	my_site.init("site_html");//html template directory
	Server sv{2000};//port number
	sv.start(my_site);//go infinite loop
}
```

> <h4>It really works, eZ Framework!!</h4>



#### - MySql Utility class and easy adoptation of various libraries

Currently, only MySql database utility class is supported. This class uses jsoncpp library. The result after executing a query will be json format. A developer can access the data with column name and row number.(Details are at [Jsoncpp Wiki](https://en.wikibooks.org/wiki/JsonCpp))

```C++
SqlQuery sq;
sq.connect("ip address", "user", "password", "database name");
sq.query("select * from table_name");
sq.fetch(10)//fetch 10 rows
sq[row]["column name"].asString();
sq[row]["column name"].asInt();
```

For other databases, developers can use appropriate library or use python or other language module. C/C++ language has long list of accumulated diverse libraries. It is a privilege to use these with simple linking. Also, Web server program can easily run script file, and call python functions using pybind etc.



#### - Scalable, Small Footprint

Middle server can distribute connections to many local web server machines. It can be achieved by editing a simple configuration text file. eZ Framework consumes little hardware. Base memory usage is less than 20M. Consequently, you can make full use of parallel connections of small computers like Raspberry Pi.



## Basic mechanism

eZ framework is based on the simple insight that web servers just get string and process it and return string. eZ framework stores all the parameters into *nameNvalue_*(map) regardless of GET, POST, multi-form file. It stores the requeseted document to variable *requeseted_document_*. If the template of requeseted document is present, it will be copied to *content_* variable. This *content_* will be sent back to browser. But before this, eZ framework will call a function to process server side procedures. This function(named process) is what developers will implement. **A developer should transform *content_* variable formed by the template within this process function, using *nameNvalue_* variable.** If there is nothing to be done in server side, no programming is needed. [Tutorial 1](https://www.zeta2374.com/tutorial1.html) is such example.



## Configuration

Edit eZ.conf to control connection distribution across local web server machines.

| domain name | local machine address | port | weight |
| ----------- | --------------------- | ---- | ------ |
| default     | 192.168.0.3           | 2002 | 1      |
| www         | 192.168.0.3           | 2002 | 1      |
| biz         | 192.168.0.3           | 2003 | 1      |
| ez          | 192.168.0.14          | 2001 | 1      |
| ez          | 192.168.0.15          | 2001 | 2      |

Middle server will read eZ.conf and distribute connections across local machines.
In this example, connections to ez domain will be distributed 1 : 2 to 192.168.0.14 machine and 192.168.0.15 machine.

#### - Middle server command line parameters

- -port : listening port(default 4433)

- -log filter : log filter setting, use lower case to disable log(default tdIWEF)

- -conf : config file(default ../eZ.conf)

- -cert : rsa certificate pem file(default ../fullchain.pem)

- -key : rsa private key pem file(default ../privkey.pem)

  

## Why eZ Framework?

## Benchmark



## Resources

[eZ Framework Officail Website](https://www.zeta2374.com/introduction.html)

[Tutorials](https://www.zeta2374.com/tutorial.html)



## License

LGPL



## Contribution

We appreciate all contributions. If you are planning to contribute back bug-fixes, please do so without any further discussion.

If you plan to contribute new features, utility functions, or extensions, please first open an issue and discuss the feature with us. 



## TLS 구현으로 배우는 암호학 : master branch 파일


#####   인증서 파일   ####

server-cert.pem : 9장 2절(Certificate장 PEM절)에서 사용된 인증서

key.pem : 22장(Certificate함수의 구현)에서 만든 키 파일
cert.pem : 22장에서 만든 인증서

ec_cert.pem : 35장 5절(TLS 1.3구현 인증서절)에서 생성한 인증서
ec_key.pem : 35장 5절(TLS 1.3구현 인증서절)에서 생성한 비밀키 파일

#####   실행 파일   ####

catch.tst.x : 테스트 실행 파일
incltouch.x : 컴파일에 필요한 유틸리티 
caesar.src.x : 카이사르 암호 실행 파일
powm_primitive.src.x : 1장의 디피헬만에서 원시근 구하는 함수
pem.src.x : PEM 파일을 출력
cert.src.x : 9장의 인증서 체인의 확인

svcl.src.x : 29장 TCPIP를 거치지 않는 테스트 실행 함수

server.src.x, client.src.x : 31장 4절 암호화되지 않은 TCPIP통신 실행 파일

tls-server.src.x, tls-client.src.x : 32장의 tls1.2 클라이언트 서버

tls13-client.src.x : 36장 1절의 tls1.3 클라이언트

https.src.x : 36장 2절 tls1.3 미들서버
site.tcpip.x : 36장 2절에서 사용한 http 서버

##### 질문 및 문의 사항은 www.zeta2374.com 을 방문해 주세요 #####

