#!/usr/bin/env python

# DNS Test Client Program

import socket
import sys
import threading

def do_it(HOST='localhost', PORT=3333, Domain='daum.net'):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((HOST, PORT))
	print "connected to server"

	sock.sendall(Domain)

	resp = sock.recv(8192)
	#if not resp:
		#break
	print '-' * 70
	print Domain, ':Response from Server-> %s' % resp
	print '-' * 70
				
	sock.close()

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print 'Usuage: python DNSClient.py HOST PORT '
		sys.exit(0)

	for i in range(10):
		do_it(sys.argv[1], int(sys.argv[2]), 'daum.net')
		do_it(sys.argv[1], int(sys.argv[2]), 'plus.net')
		do_it(sys.argv[1], int(sys.argv[2]), 'naver.com')
		do_it(sys.argv[1], int(sys.argv[2]), 'mit.edu')
		do_it(sys.argv[1], int(sys.argv[2]), 'anydic.com')
		do_it(sys.argv[1], int(sys.argv[2]), 'anydic.com')
	


