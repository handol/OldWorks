#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# PAS client
import socket
import time


####
def connPAS(host='localhost', port=50007, debug=0):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setblocking(1)
	if debug: print "after socket(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	#s.bind(("192.168.2.6", port))
	#s.bind((host, port))
	if debug: print "after bind(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	s.connect((host, port))
	if debug: print "after connect(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	if debug: print "peer name: %s %d" % (s.getpeername()[0], s.getpeername()[1])
	return s


####
def sendtoPAS(s, msg):
	s.send(msg)
	
	start = time.time()
	data = ''
	while 1:
		time.sleep(0.2)
		try:
			one = s.recv(1024)
			if len(one)>0:
				print one
			elif len(one)==0:
				print "conn closed"
				break
			data += one
		except:
			break
		
	end = time.time()
	print "LENGTH=%d" % (len(data))
	print "lap time = %d" % (end - start)
	time.sleep(0.9)
	time.sleep(0.9)
	time.sleep(0.9)



#User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
#HTTP_CHANNEL_INFO:CH:A
#HTTP_MDN_INFO:%s

ODDMSG = "HTTP_DEVICE_INFO:LX:176,LY:220,CL:8\n"

REQMSG = \
"""GET %s HTTP/1.1
Host: %s
User-Agent: Mozilla/1.22 (compatible; KUN/1.0; ; CellPhone)
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0
HTTP_PHONE_NUMBER: 82%s
HTTP_DEVICE_INFO:LX:176,LY:220,CL:8
HTTP_DRIVER_INFO:IMG:MSIS|NBMP,SND:MA3|SMAF|MA5
HTTP_PLATFORM_INFO:PNAME:BREW,PVER:V1.2,PID:1002
HTTP_PLATFORM_INFO2:PNAME:WOB,PVER:V1.0,PID:1000
HTTP_MNC_INFO:04
COUNTER:2
Cookie: SITESERVER=ID=958c5b3e4ce473494a6db82d70887570;
Proxy-Connection: Keep-Alive
Accept: */*
Accept-Language: en
Accept-Encoding: deflate\r
\r
"""
#Accept-Encoding: deflate


if __name__ == "__main__":
	import sys
	import urlparse

	HOST = 'localhost'    # The remote host


			
	if len(sys.argv) >= 4:
		PORT = int(sys.argv[1])
		MDN = sys.argv[2]
		URL = sys.argv[3]
	else:
		print "pas_port MDN URL"
		sys.exit(0)


	scheme, host, path, params, query, fragment =\
		urlparse.urlparse(URL)

	print "## Paras: ", HOST, URL, host, MDN
	msg = REQMSG % (URL, host, MDN)	

	sock = connPAS(host=HOST, port=PORT)
	sock.send(msg)
	sock.send("0000" + msg)
	sendtoPAS(sock, ODDMSG + msg)
	
	sock.close()

	
	
