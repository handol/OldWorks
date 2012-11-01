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
def sendtoPAS(s, msg1, msg2, msg3, msg4):
	s.send(msg1)
	s.send(msg2)
	s.send(msg3)
	s.send(msg4)
	
	start = time.time()
	data = ''
	while 1:
		try:
			one = s.recv(1024)
			#if not one: break
			#if len(one) < 1: break
			print one
			data += one
		except:
			break
		
	#print data
	end = time.time()
	print "LENGTH=%d" % (len(data))
	print "lap time = %d" % (end - start)




REQMSG = \
"""GET %s HTTP/1.1
Host: %s
User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
COUNTER:1
HTTP_PHONE_NUMBER: %s
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0
HTTP_DEVICE_INFO:LX:176,LY:220,CL:8
HTTP_DRIVER_INFO:IMG:MSIS|NBMP,SND:MA3|SMAF|MA5
HTTP_PLATFORM_INFO:PNAME:BREW,PVER:V1.2,PID:1002
HTTP_CHANNEL_INFO:CH:A
HTTP_MNC_INFO:04
HTTP_MDN_INFO:%s
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

	if len(sys.argv) < 2:
		print "Enter PAS-server's port number"
		sys.exit()

	HOST = 'localhost'    # The remote host
	PORT = int(sys.argv[1])

	MDN = "820163331234"


	msg1 = REQMSG % ("http://anydic.com/sleep5.dic", "anydic.com", MDN, MDN)	
	msg2 = REQMSG % ("http://anydic.com/sleep1.dic", "anydic.com", MDN, MDN)	
	msg3 = REQMSG % ("http://localhost/cgi-bin/sleep35.cgi", "localhost", MDN, MDN)	
	msg4 = REQMSG % ("http://localhost/cgi-bin/sleep1.cgi", "localhost", MDN, MDN)	

	sock = connPAS(host=HOST, port=PORT)
	sendtoPAS(sock, msg1, msg2, msg3, msg4)
	sock.close()
	
