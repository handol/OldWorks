#!/usr/bin/env python

import socket
import sys

def do_it(HOST='', PORT=15469):
	'simple echo udp client'
	#HOST = 'localhost'
	#HOST = '221.148.247.32'
	#HOST = '221.148.247.20'
	BUFSIZE = 1024
	ADDR = (HOST, PORT)

	udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

	while True:
		data = raw_input('$ ')
		if not data: break
		udpsocket.sendto(data, ADDR)
		data, ADDR = udpsocket.recvfrom(BUFSIZE)
		if not data: break
		print data
	udpsocket.close()

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print 'Usage: python udpClient.py host port(15469)'
		sys.exit(0)
	do_it( sys.argv[1], int(sys.argv[2]) )

	
