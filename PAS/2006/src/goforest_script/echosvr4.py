#! /usr/bin/env python

# Python implementation of an 'echo' tcp server: echo all data it receives.
#
# This is the simplest possible server, servicing a single request only.

import sys
import time
from socket import *

# The standard echo port isn't very useful, it requires root permissions!
# ECHO_PORT = 7
ECHO_PORT = 8888
BUFSIZE = 1024

def main():
	conn, (remotehost, remoteport) = s.accept()
	print 'connected by', remotehost, remoteport

	data = conn.recv(1024)

	print "client data : %s" % (data)

#	data = "HTTP/1.1 200 OK\r\nDate: Tue, 10 Oct 2006 13:25:22 GMT\r\nServer: Apache/2.2.0 (Unix) DAV/2 mod_ssl/2.2.0 OpenSSL/0.9.8a\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nExpires: -1\r\nTransfer-encoding: chunked\r\nContent-Type: text/html\r\n\r\n"
	data = "HTTP/1.1 200 OK\r\nDate: Tue, 10 Oct 2006 13:25:22 GMT\r\nServer: Apache/2.2.0 (Unix) DAV/2 mod_ssl/2.2.0 OpenSSL/0.9.8a\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nExpires: -1\r\nContent-length: 36\r\nContent-Type: text/html\r\n\r\n"
	conn.send(data)
	print "send data : %s" % (data)

	time.sleep(0.1)
	data = "1234567890abcdef1234567890\r\n";
	conn.send(data)
	print "send data : %s" % (data)

#	time.sleep(35)

#	time.sleep(0.1)
#	data = "1234567890abcdef";
#	conn.send(data)
#	print "send data : %s" % (data)

#	time.sleep(0.1)
#	data = "00\r\n"
#	conn.send(data)
#	print "send data : %s" % (data)

	conn.close()
	print "child end"

if len(sys.argv) > 1:
	port = int(eval(sys.argv[1]))
else:
	port = ECHO_PORT

s = socket(AF_INET, SOCK_STREAM)
s.bind(('', port))
s.listen(1)
while 1:
	main()

