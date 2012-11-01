#!/usr/bin/env python

# Ftp Client Program

import socket
import sys

def do_it(HOST='localhost', PORT=3333, fileName='mylog'):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((HOST, PORT))
	print "connected to server"

	fp = open(fileName, 'r')
	content = fp.read()
	fileSize = len(content)

	if len(fileName) > 32:
		print 'Too Long Filename'
		sys.exit(0)
				
	fileName = '%32s' % fileName
	fileSize = '%016d' % fileSize
	
	CON = fileName + fileSize + content
	sock.sendall(CON)

	resp = sock.recv(8192)
	#if not resp:
		#break
	print '-' * 70
	print 'Response from Server: %s size file' % resp
	print '%s Upload Success' % fileName.strip()
	print '-' * 70
				
	sock.close()
	fp.close()

def makefile():
	# Temp Function
	fp = open('mylog', 'w')
	for i in range(0, 10):
		fp.write(repr(i))

	fp.close()

if __name__ == '__main__':
	if len(sys.argv) != 4:
		print 'Usuage: python ftpClient.py HOST PORT Filename'
		sys.exit(0)
	
	do_it(sys.argv[1], int(sys.argv[2]), sys.argv[3])
	

