# Echo server program
import socket
import sys
import os

HOST = ''                 # Symbolic name meaning the local host
PORT = 8888              # Arbitrary non-privileged port
if len(sys.argv) > 1: 
	PORT = int(sys.argv[1])


def sock_svr(s):

	data = conn.recv(1024)

	print "client data : %s" % (data)

	data = "HTTP/1.1 200 OK\r\nDate: Tue, 10 Oct 2006 13:25:22 GMT\r\nServer: Apache/2.2.0 (Unix) DAV/2 mod_ssl/2.2.0 OpenSSL/0.9.8a\r\nCache-Control: no-cache\r\nPragma: no-cache\r\nExpires: -1\r\nTransfer-encoding: chunked\r\nContent-Type: text/html\r\n\r\n"
	conn.send(data)

	data = "0A"
	conn.send(data)

	data = "1111111111";
	conn.send(data)

#	data = "0A"
#	conn.send(data)

	data = "0"
	conn.send(data)

	conn.close()
	exit()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
print "bind(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

s.listen(5)

while 1:
	#s2 = s.dup()
	conn, addr = s.accept()

	print 'Connected by', addr
	print "accept(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])
	print "client: fd=%d, addr=%s, %d" % (conn.fileno(), conn.getsockname()[0], conn.getsockname()[1])

	pid = os.fork()
	if pid==0:
		#child
		sock_svr(s)

