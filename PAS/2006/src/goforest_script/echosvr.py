# Echo server program
import socket
import sys
import os

HOST = ''                 # Symbolic name meaning the local host
PORT = 50007              # Arbitrary non-privileged port
if len(sys.argv) > 1: 
	PORT = int(sys.argv[1])


def sock_svr(s):
	s.listen(5)
	conn, addr = s.accept()
	print 'Connected by', addr
	print "accept(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	print "client: fd=%d, addr=%s, %d" % (conn.fileno(), conn.getsockname()[0], conn.getsockname()[1])

	while 1:
		data = conn.recv(1024)
		print "Received Data : %s" % (data)

		data = "1234567"
		if not data: break
		conn.send(data)
	conn.close()
	sys.exit()

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
print "bind(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

s2 = s.dup()
pid = os.fork()
if pid==0:
	#child
	sock_svr(s)
else:
	#parent
	sock_svr(s)


