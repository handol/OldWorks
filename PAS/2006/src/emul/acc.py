#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# PAS client
import socket
import time


def conntest(host, port):
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	s.setblocking(1)
	before = time.time()
	s.connect((host, port))
	after = time.time()
	print "%s conn: %d" % (time.ctime(), s.fileno())
	if after - before > 1:
		print "conn DELAY: %d"  %( after-before)
	s.close()

if __name__ == "__main__":
	import sys

	if len(sys.argv) >= 3:
		PORT = int(sys.argv[2])
		HOST = sys.argv[1]


	while 1:
		conntest(host=HOST, port=PORT)
		time.sleep(5)

	
	
