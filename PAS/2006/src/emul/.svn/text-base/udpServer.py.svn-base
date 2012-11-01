#!/usr/bin/env python

### UDP Echo Server

###

import sys
import socket

def do_it(PORT=15469):
	'simple echo udp server'

	BUFSIZE = 1024

	udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	#udpsocket.bind( ('221.148.247.32', 15469) )
	udpsocket.bind( ('', PORT) )

	while True:
		print 'waiting.. '
		data, addr = udpsocket.recvfrom(BUFSIZE)
		udpsocket.sendto(data, addr)
		print 'received from', addr, 'data:', data

	udpsocket.close()

if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'Usage: python udpServer.py port(15469)'
		sys.exit(0)
	do_it( int(sys.argv[1]) )



