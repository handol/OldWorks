#!/usr/bin/env python

import sys
import time
import socket

import util


def test_dict():
	while 1:
		a = util.getinput("Enter a word:")
		if a=='.': break

		req_dic(a)
		
def req_dic(word):
	msg = "%-4d%s" % (len(word), word)
	sock.send (msg)
	buf = sock.recv (1024*4)
	print buf

if __name__=='__main__':
	if len(sys.argv) >= 2:
		port = int(sys.argv[1])
	else:
		port = 8900

	print "port=",port
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	try:
		sock.connect ( (socket.gethostbyname('localhost'), port) )
	except:
		print 'connection failed !!'
		sys.exit()


	test_dict()
