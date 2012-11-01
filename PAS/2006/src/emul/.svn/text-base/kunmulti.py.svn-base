#!/usr/bin/env python # -*- coding: EUC-KR -*-
# PAS client
import socket
import time
import threading

#User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
#Accept-Encoding: deflate
REQMSG = \
"""GET %s HTTP/1.1
Host: %s
User-Agent: Mozilla/1.22 (compatible; KUN/2.0; KTF5000; CellPhone)
COUNTER:1
HTTP_PHONE_NUMBER: 82%s
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

class KunClient(threading.Thread):
        def __init__(self, host='localhost', port=50007, id=0, debug=0):
                threading.Thread.__init__(self)
                #self.header = Header()                                          # send Header
                #self.recheader = RecHeader()                                    # received Header
                self.host = host
                self.port = port
                self.debug = debug
                self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.id = id
                
	def makeMsg(self, URL, host):
		MDN = '%04d' % self.id
		self.msg = REQMSG % (URL, host, '0100000'+ MDN, '0100000' + MDN)	

        def run(self):
                time.sleep(5)
                print "runned..", threading.currentThread()

        def conn(self):
                self.sock.setblocking(1)
                if self.debug: print "after socket(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])

                #s.bind(("192.168.2.6", port))
                #s.bind((host, port))
                
                if self.debug: print "after bind(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])
                
                #try: 
                self.sock.connect((self.host, self.port))
                #except:
                        #print 'connection failed !!'
                        
                if self.debug: print "after connect(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])

                if self.debug: print "peer name: %s %d" % (self.sock.getpeername()[0], self.sock.getpeername()[1])

                return 1        # return 1 if success

        def sendmsg(self):
                self.sock.send(self.msg)

	def recvmsg(self):
                start = time.time()
                data = ''
		while len(data) == 0:
			try:
				one = self.sock.recv(1024)
				#if not one: break
				#if len(one) < 1: break
				data += one
			except:
				continue	

                end = time.time()
                print "LENGTH=%d" % (len(data))
                print "lap time = %d" % (end - start)
                return data
		
        def handleRespond(self, result):
                pass

        def close(self):
                self.sock.close()
####

if __name__ == "__main__":
	import sys
	import urlparse

	if len(sys.argv) != 5:

		print 'Usuage: python testkunclient.py pas_addr pas_port num_client url'
		sys.exit(0)

	HOST = sys.argv[1]
	PORT = int(sys.argv[2])
	num = int(sys.argv[3])
	URL = sys.argv[4]

	scheme, host, path, params, query, fragment =\
		urlparse.urlparse(URL)


	print "## Paras: ", HOST, URL, host
	#msg = REQMSG % (URL, host, MDN, MDN)	

	kunclient = []
	for i in range(num):
		client = KunClient(HOST, PORT, i, 0)
		client.makeMsg(URL, host)
		client.conn()
		kunclient.append(client)
		time.sleep(0.01)

	for i in range(num):
		kunclient[i].sendmsg()
		#print kunclient[i].msg
	while True:
		time.sleep(1)




