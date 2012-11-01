#!/usr/bin/env python

import sys
import time
import SocketServer


def makeHttpResp(body):
	msg = "HTTP/1.0 200 OK\n"
	msg += "Content-type: text/html;charset=euc-kr\n"
	msg += "Content-Length: %d\n" % (len(body))
	msg += "\n"
	msg += body
	return msg

def prn_sockinfo(sock):
	print "fd= ", sock.fileno()
	print "addr= ", sock.getsockname()
	print "peer= ", sock.getpeername()


class MyHandler(SocketServer.BaseRequestHandler):
	def handle(self):
		prn_sockinfo(self.request)

		while 1:
			data = self.request.recv(1024)
			print 'Read %d bytes [%d]' % (len(data), self.request.fileno() )
			print data
			tosend = makeHttpResp(data)
			print 'Sending %d bytes' % len(tosend)
			self.request.send(tosend)
			break
			if not data: 
				## means peer closed
				break


if __name__=='__main__':
	if len(sys.argv) >= 2:
		port = int(sys.argv[1])
	else:
		port = 50007

	#myServer = SocketServer.TCPServer(('',8080), MyHandler)
	myServer = SocketServer.ThreadingTCPServer(('',port), MyHandler)
	myServer.serve_forever( )

