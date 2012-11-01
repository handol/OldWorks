# Echo client program
import socket
import time

HOST = 'localhost'    # The remote host
PORT = 50007              # The same port as used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print "after socket(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

port = 0

if __name__ == "__main__":
	import sys
	try:
		port = int(sys.argv[1])
	except:
		port = PORT
		#pass
	
#s.bind(("192.168.2.6", port))
#s.bind((HOST, port))
print "after bind(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

s.connect((HOST, PORT))
print "after connect(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

print "peer name: %s %d" % (s.getpeername()[0], s.getpeername()[1])

while 1:
	s.send('Hello, world')
	data = s.recv(1024)
	print "Received %s"  % (data)

	time.sleep(0.05)	
	if data == "0": break
	if data == "": break

s.close()

	
	
