#!/usr/bin/env python
# PAS client
import socket
import time
import select


####
def connPAS(host='localhost', port=50007, debug=0):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	#s.setblocking(0)
	if debug: print "after socket(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	#s.bind(("192.168.2.6", port))
	#s.bind((host, port))
	if debug: print "after bind(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	s.connect((host, port))
	if debug: print "after connect(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	if debug: print "peer name: %s %d" % (s.getpeername()[0], s.getpeername()[1])
	return s


####
def recvfromPAS(s, waitTime):
	handles = [s.fileno()]
	try:
		reads, writes, in_erros = select.select(handles, [], [], waitTime)
	except:
		reads = []

	data = ""
	if len(reads) > 0:
		one = sock.recv(1024*64)
		data += one
		print data
		print "LENGTH=%d" % (len(data))
	else:
		print "nothing to recv"



def sendtoPAS(s, msg):
	s.send(msg)
	time.sleep(0.1)
	recvfromPAS(s, 2)

	s.send(msg)
	time.sleep(0.1)
	recvfromPAS(s, 2)

	while 1:
		time.sleep(1)


REQMSG = \
"""GET %s HTTP/1.0
User-Agent: Mozilla/1.22 (compatible; MSMB111; KTF4016; CellPhone)
COUNTER:1
HTTP_PHONE_NUMBER: 82%s
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:345,NID:24,SID:2180,BASE_LAT:539768,BASE_LONG:1829452
Accept:
Host: %s
Accept-Language: kr
Cookie: SITESERVER=ID=437cb9f6ba45ee509edc9230290b8fe1

"""


if __name__ == "__main__":
	import sys
	import urlparse

			
	if len(sys.argv) >= 4:
		PORT = int(sys.argv[1])
		MDN = sys.argv[2]
		URL = sys.argv[3]
	else:
		print "Usage: PAS_PORT MDN URL"
		sys.exit()


	HOST = "localhost"
	scheme, host, path, params, query, fragment =\
		urlparse.urlparse(URL)

	print "## Paras: ", HOST, URL, host, MDN
	msg = REQMSG % (URL, MDN, host)	

	sock = connPAS(host=HOST, port=PORT)
	sendtoPAS(sock, msg)
	sock.close()

	
	
