<img src=https://tomcat.zeta2374.com/image/ez.png height=200>

# eZ Framework는 C++ Server Side Web Development Framework입니다.




## Try your first eZ Framework

```html
<html>
<h1>It works eZ Framework!!</h1>
</html>
```

```c++
#include"tcpip/server.h"
#include"tcpip/website.h"

int main() {
	WebSite my_site;
	my_site.init("site_html");//html template directory
	Server sv{2000};//port number
	sv.start(my_site);//go infinite loop
}
```

### access localhost port 2000 =>  Result

<h1>It works eZ Framework!!</h1>



## Branch

- master : TLS 구현으로 배우는 암호학
- opensw : 공개 SW 개발자 대회
- develop : 최신 브랜치



## Directory Structure

- 기본적인 eZ 프레임워크 파일들
- middle : eZ Framework의 미들 서버
- lib : eZ Framework의 여타 기능
- incltouch : 컴파일용 유틸리티
- eZ.conf 파일 : eZ 프레임워크 configuration 파일
- run.sh : 이지 프레임워크와 사이트들을 론칭하는 배시 스크립트
- cert.pem, key.pem : 이지 프레임워크에서 사용하는 인증서 파일

eZ 프레임워크를 이용하여 만든 웹사이트들
dndd : 게시판 사이트
biz : 명함관리 사이트
ez : 메인사이트
adnet : 배너광고 사이트
suwon : 수원지역 지역화폐 가맹점 검색 사이트



## Install



인스톨 방법

eZ Framework의 dependency
sudo apt install libjsoncpp-dev nettle-dev libgmpxx4ldbl

명함 관리 웹 앱의 경우 opencv와 gensim을 추가로 인스톨해야 한다.
biz 디렉토리 : 명합관리 웹 앱의 dependency
sudo apt install libopencv-dev tesseract-ocr tesseract-ocr-eng tesseract-ocr-kor 
pip3 install gensim

make 명령어를 루트 디렉토리에서 실행하면 모든 실행파일이 빌드된다.
만일 실패할 경우, make safe를 먼저 실행해 보자.
그 이후 다시 make을 실행해 본다.
혹은 빠진 라이브러리를 설치해 본다.



## Features

1. Liabrary is to be used, not to be learned : Very easy
2. Virtual programming environment with State
3. Middle Server
4. Easy Template Usage
5. MySql Utility class and easy adoptation of various libraries

There are so many things to learn and memorize.
					We don't want to add one more burden.
					eZ Framework is a very easy server side web development framework.
					You just need to memorize just a few things to develop with eZ framework.
					The rest depends on the developer's adaptability.
					eZ framework provides very simple web development environment.
					Developer can just think that the program is exchanging messages with one connector.
					Web site is defined by a class so that structured programming is very convinient.



## 기본 메커니즘

