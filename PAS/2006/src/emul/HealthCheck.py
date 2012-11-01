#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# PAS L4 Health Check
import socket
import time


####
def connPAS(host='localhost', port=50007, debug=0):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setblocking(1)
	if debug: print "after socket(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	s.connect((host, port))
	print "#Connected"
	if debug: print "after connect(): fd=%d, addr=%s, %d" % (s.fileno(), s.getsockname()[0], s.getsockname()[1])

	if debug: print "peer name: %s %d" % (s.getpeername()[0], s.getpeername()[1])
	return s


if __name__ == "__main__":
	import sys

	#HOST = 'ktfkunproxy.ktf.com'    # The remote host
	HOST = 'localhost'    # The remote host
	PORT = 50007              # The same port as used by the server

	if len(sys.argv) < 2 or sys.argv[1]=='help' or sys.argv[1]=='?':
		print "usage: python HealthCheck.py [port]"
		sys.exit()
	else:
		PORT = int(sys.argv[1])

	print "#Host: ", HOST
	print "#PORT: ", PORT

	sock = connPAS(host=HOST, port=PORT)
	sock.close()
	print "#Connection closed"
