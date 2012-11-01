#!/usr/bin/env python
# -*- coding: EUC-KR -*-

# Ftp Client Program V.0305
# Directory Option 을 사용할 경우 절대 경로 또는 상대 경로 사용

import socket
import sys

def do_it(METHOD = 'put', HOST='localhost', PORT=3333, fileName='mylog', option='nooption'):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((HOST, PORT))
	print "connected to server"

	method = '%4s' % METHOD

	if METHOD == 'put':
		fp = open(fileName, 'r')
		content = fp.read()
		fileSize = len(content)

		if fileName.find('/') != -1:	# file 이름만 넘김.
			seq = fileName.split('/')
			savedfilename = seq.pop()
		else:
			savedfilename = fileName

		if len(savedfilename) > 128:	# 128 byte 파일이름 제한 
			print 'Too Long Filename'
			sys.exit(0)

		if option == 'nooption':
			optionheader = '%128s' % 'no_directory_option'
		else:
			optionheader ='%128s' %  option

		print optionheader
		savedfilename = '%128s' % savedfilename
		fileSize = '%016d' % fileSize
		
		CON = method + savedfilename + fileSize + optionheader + content
		sock.sendall(CON)

		resp = sock.recv(8192)
		#if not resp:
			#break
		print '-' * 70
		print 'Response from Server: %s size file' % resp
		print 'Upload Success: %s' % fileName.strip()
		print '-' * 70
					
		sock.close()
		fp.close()

	elif METHOD == 'get':
		if fileName.find('/') != -1:
			seq = fileName.split('/')
			savedfilename = seq.pop()
		else:
			savedfilename = fileName


		CON = method + fileName

		sock.sendall(CON)	# Send: GET + FileName 

		resp = sock.recv(8192)  # Recv: the size of File

		if resp.find('There is not the file') != -1:
			print 'There is not the file'
			sys.exit()

		length = int( resp [ 0 : 16 ] )
		resp = resp [ 16 : ]
		current = len(resp)

		fp = open(savedfilename, 'w')
		while True:
			if current < length:
				fp.write(resp)
				resp = sock.recv(8192)
				if not resp:
					print 'no data'
					break
				current = current + len(resp)
			elif current > length:
				fp.write( resp [ : - (current - length) ] )
				fp.close()
				break
			elif current == length:
				fp.write(resp)
				fp.close()
				break
				
		sock.sendall( repr(length) )

		print '-' * 70
		print 'Response from Server: %s size file' % length
		print 'Download Success %s' % fileName.strip()
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
	#if len(sys.argv) != 5:
		#print 'Usuage: python pasFtpClient.py Method(get,put) HOST PORT Filename'
		#sys.exit(0)

	argv_length = len(sys.argv)

	if argv_length != 5 and argv_length != 6:
		print 'Usuage: python pasFtpClient.py Method(get,put) HOST PORT Filename (Directory)'
		sys.exit(0)

	if sys.argv[1] != 'get' and sys.argv[1] != 'put':
		print 'Usuage: python pasFtpClient.py Method(get,put) HOST PORT Filename (Directory)'
		sys.exit(0)

	if argv_length == 5:
		do_it(sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4], 'nooption')
	elif argv_length == 6:
		do_it(sys.argv[1], sys.argv[2], int(sys.argv[3]), sys.argv[4], sys.argv[5])
	else:
		print 'Usuage: python pasFtpClient.py Method(get,put) HOST PORT Filename (Directory)'
		sys.exit(0)
	


