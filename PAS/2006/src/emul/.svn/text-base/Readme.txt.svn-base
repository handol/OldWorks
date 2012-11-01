**공지 처리 테스트
  #[KUN] python stattest.py k_stat.cfg [n]
  #[ME] python stattest.py stat.cfg [n]
  [n] 은 테스트 개수.
  주어진 stat.cfg 에서 comment out 된 공지 처리까지 모두 포함하여 테스트.

kunclient.py
	KUN 브라우저 에뮬. 기본 기능만 있음. 
	한번만  Request 보내고 응답 받음.
	socket 을 blocking 또는 non-blocking으로 세팅하기 위해서는  소스에서 세팅 필요.
	REQ header의 내용을 변경시켜 가며 (소스 수정을 통해) 예외 케이스를 테스트 할 수 있다.

meclient.py
	ME 브라우저 에뮬. 기본 기능만 있음. 
	한번만  Request 보내고 응답 받음.
	
pipelinging.py
pipe2.py
	PAS2006서버의 파이프라이닝 기능을 테스트하기 위한 단말 에뮬.

디렉토리 - chunk
	PAS2006 서버의 Chunked data (CP RESP body)를 처리 기능 중 예외 상황 처리를 확인하기
	위해서 만든 것으로, CP web server를 흉내내며 Chunked 형태의 CP response를 응답한다.
	예외 내용:
		Chunked 길이보다 내용이 길거나 짧은 경우.
		Chunked 이면서 Content-Length가 포함된 경우.
		Content-Length 값보다 내용이 길거나 짧은 경우.
		
AcceptWithDelay.py
	실행 파라미터는 서버포트 번호와 delay_sec를 받는다.  
	acceptWithDelay.py 는 간단한 웹서버 같은 기능한다. 다만 항상 똑 같은 응답을 한다.
	동시에 한 클라이언트만 처리한다. tcp server를 만들되, listen() 하다가, accept()를 2~3 초 후에 
	accept()후에 read()하고, 무조건 정의된 http mesg만 send()한다.

	정의된 메세지 = \
	"""\
	HTTP 200 OK\r\n
	Content-Length: 20\r\n
	\r\n
	<html><body>Good</body></html>
	"""


complog.py
	1) 통계로그경우
	python complog.py -s oldfile newfile
	하게 되면 두 파일의 로그를 서로 비교하고 같은 URL요청인데 내용이 다를경우 다른 항목(field)을
	각각 출력해줍니다.

	2) 과금로그경우
	python complog.py -m oldfile newfile

	python complog.py 하시면 같은 내용을 보실 수 있습니다.


ftpServer.py
	서버로 쓸 머신에서 python ftpServer.py PORT 로 실행하시면 됩니다.
	서버에서 저장되는 파일은 Client에서 올리는 파일이름과 동일합니다. 


ftpClient.py
	클라이언트로 쓸 머신에서 python ftpClient Host PORT Filename
	하시면 목표 HOST, PORT에 Filename 이 저장됩니다. 성공하면 Success라는 메세지와 파일명, 
	사이즈가 뜹니다.;;
	ftp 프로그램 둘다 둘다 python 실행화일.py 하시면 사용법을 보실수 있습니다.


DnsCache.py 
DNSClient.py

	1. 요약
	DNS Query Program 입니다. Query 하는데 0.8초이상 걸리면 0.0.0.0 을 Return 합니다. :)
					       0.8초이하 걸리면 IP를 Return합니다.
	2. 사용법
	 python DnsCache.py 포트번호
	 
	3. Test Driven...
	테스트를 위해서 DNSClient.py 를 만들었습니다.
	http://anydic.com/svn/PAS2006/trunk/src/emul/DNSClient.py
	사용방법은
	python DNSClient.py 목표호스트 목표포트

	아래처럼 됩니다.
	 
	python DNSClient.py localhost 3333
	connected to server
	----------------------------------------------------------------------
	anydic.com :Response from Server-> 218.153.100.91
	----------------------------------------------------------------------




디렉토리 - benchmark
	PAS2006 에 대한 성능 검증용 도구.
	urlList.txt에 있는 URL 목록을 바탕으로 여러 개의 단말을 에뮬하며, PAS 서버에 부하를 가하고,
	그 결과 통계를 출력한다.

디렉토리 - simplebench
	PAS2006 에 대한 성능 검증용 도구. benchmark/* 를 약간 수정한 것.
	단말의 갯수를 주기적으로 증가시킬 때 기존의 단말 thread 들을 재기동(종료 후 다시 시작)하지 않고,
	새 쓰레드만 추가하는 방식으로 변경한 것이다.
   
=== santaHttpServer.py  ,   santa.acl   ===

가상번호 == MIN : 단말의 browser에 담겨 올라오는 번호.가상번호는 01690xx, 01691xx 대역입니다.
MDN: 실제 번호, 사용자가 자기번호로 알고 있는 일반 번호.
SANTA: 가상번호 (MIN번호)에 해당하는 MDN을 저장하고, 조회할 수 있도록 제공하는 서버.

ME 단말테스트를 진행할 때 대부분의 ME단말이 가상번호이기 때문에 santa 조회가 필요합니다.

그런데 TB에서는 상용 SANTA에 연결할 수 없고,
TB SANTA가 있으나, 그쪽에 저장되어 있는 MIN번호가 특정 대역이라서,
우리가 단말로 테스트 하는데 사용하기 어렵다.

그래서 우리가 만든 santa emul 있다.

=== santa emul 사용법.
http://zetamobile.com/svn/PAS2006/trunk/src/emul/santaHttpServer.py
http://zetamobile.com/svn/PAS2006/trunk/src/emul/santa.acl    -- santa 접속 암호,  MIN <--> MDN 매핑 데이타 를 담고 있다.

사용법 : 아래와 같이 기동 (port 30001)하고, ME/KUN 의 컨피그에도 santa 포트 및 암호를 맞게 지정하여야 한다.
# python santaHttpServer.py 30001  santa.acl


== santa.acl 내용
# SANTA 접속 암호 등등

SVCID mapexam
ID pas
PASSWORD pas

#  MIN(virtual)   IMSI          MDN
01690787300 420001690787300 01032166366
01691971624  420001691971624 01000001624
1691971624  420001691971624 01000001624
1690054236 420001690054236 01000004236
01690054236 420001690054236 01000004236

