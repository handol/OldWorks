#!/usr/bin/env python

import sys
import time
import socket

def do_it(Domain):
	start = time.time()

	socket.gethostbyname(Domain)

	end = time.time()
	
	return end - start 


if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'Usuage: python Dnstest.py DOMAIN'
		sys.exit(0)
	while True:
		result = do_it(sys.argv[1])
		print sys.argv[1], 'Query Time:',
		print '%6f' % result
		time.sleep(1)



