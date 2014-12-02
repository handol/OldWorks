1.	libcurl 설명
A.	FTP, HTTP 등의 다양한 프로토콜을 지원하는 open source 라이브러리이다. Windows 및 Unix를 포함한 다양한 OS 플랫폼에 포팅되었다.
B.	홈 - http://curl.haxx.se/libcurl/
2.	다운받기
C.	http://curl.haxx.se/download.html 에서
D.	curl-7.13.2.zip 을 받아 압축을 풀고 컴파일 하면 된다. 파일 크기는 2,555Kb 가량이다.
3.	컴파일
E.	curl-7.13.2.zip 를 D:\ad2005\curl-7.12.3 에 풀었다고 가정하자.
F.	D:\ad2005\curl-7.12.3\lib\curllib.dsw 파일을 열어 Visual Studio에서 Rebuild All을 수행한다.
G.	D:\ad2005\curl-7.12.3\lib\Debug 에 dll 및 lib 파일이 생성 된다.
i.	libcurl.dll
ii.	libcurl.lib

H.	D:\ad2005\netlib\ 아래에  curl원본의 include/curl 디렉토리를 복사한다.
i.	D:\ad2005\netlib\curl-7.13.2\curl-7.13.2\include\curl

4.	라이브러리 설치
H.	라이브러리를 다른 소스에서 link하여 쓸 수 있도록 환경 변수에 라이브러리 파일이 위치하는 path 를 추가하여야 한다.
I.	libcurl.dll, libcurl.lib 을 D:\ad2005\extlib\ 에 복사한다.
J.	환경변수 중 ‘PATH’값에 D:\ad2005\extlib\ 을 추가한다.


### 2006.6m : curl-7.15.4.zip
