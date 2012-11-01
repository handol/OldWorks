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
	
def recvfromPAS(s):
	print ".... wait to recv"
	start = time.time()
	data = ''
	while 1:
		try:
			one = s.recv(1024)
			if len(one)>0:
				data += one
				print one
				break
		except:
			break
	
	end = time.time()
	print "LENGTH = %d" % (len(data))
	print "lap time = %d" % (end - start)



def sslConnMesg(sslhost, sslport):
	SSLCONN = \
"""CONNECT %s:%d HTTP/1.1
Host: %s:%d

"""
	return SSLCONN % (sslhost, sslport, sslhost, sslport)

def sslResultMesg():
	SSLRESULT = \
"""RESULT
CPData: cpname=woori;svccode=wooribanking000
User-Agent: MobileExplorer/1.2 (Mozilla/1.22; compatible; KUNF12;
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0

"""
	return SSLRESULT

def sslDataMesg():
	SSLDATA = \
"""!@#$%^&*() 1234567890"""
	return SSLDATA


#User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
REQMSG2 = \
"""CONNECT 220.73.3.241:9162 HTTP/1.1
Host: 220.73.3.241:9162

"""

REQMSG = \
"""CONNECT ent.wooribank.com:443 HTTP/1.1
Host: ent.wooribank.com:443

"""


REPORT_MSG = \
"""RESULT
CPData: cpname=woori;svccode=wooribanking000
User-Agent: MobileExplorer/1.2 (Mozilla/1.22; compatible; KUNF12;
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0

"""

#HTTP_PHONE_NUMBER: 8201073989200

#Accept-Encoding: deflate


if __name__ == "__main__":
	import sys
	import urlparse

	if len(sys.argv) < 5:
		print "usage: pas_addr pas_port ssl_host ssl_port"
		sys.exit()

	HOST = sys.argv[1]
	PORT = int(sys.argv[2])
	SSLHOST = sys.argv[3]
	SSLPORT = int(sys.argv[4])

	sock = connPAS(host=HOST, port=PORT)

	sendtoPAS(sock, sslConnMesg(SSLHOST, SSLPORT))
	recvfromPAS(sock)
	time.sleep(1)

	
	sendtoPAS(sock, sslDataMesg())
	recvfromPAS(sock)
	time.sleep(1)

	
	sendtoPAS(sock, sslResultMesg())
	recvfromPAS(sock)
	time.sleep(1)

