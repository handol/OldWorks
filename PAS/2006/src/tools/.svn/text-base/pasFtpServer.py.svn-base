#!/usr/bin/env python
# -*- coding: EUC-KR -*-

# Ftp Server Program V.0305

import socket
import SocketServer
import sys
import time
import os

HTTP_HEAD = \
"""HTTP/1.1 200 OK
Server: Apache/2.0.52 (Unix) mod_ssl/2.0.52 OpenSSL/0.9.7d DAV/2 SVN/1.3.0 PHP/4.3.8
Connection: close
Content-Type: text/html; charset=EUC-KR
Content-Length: %d

"""

HTML_MSG = \
"""<HTML>
<BODY> It works ! </BODY>
</HTML>
"""

HTTP_MSG = HTTP_HEAD % ( len(HTML_MSG) ) +  HTML_MSG

print HTTP_MSG
class ftpHandler(SocketServer.BaseRequestHandler):
	''' Threaded Server 구현을 위해서 '''

	def handle(self):
		print "Connected from", self.client_address
		while True:
			receivedData = self.request.recv(8192)
			if not receivedData: break

			self.request.sendall(receivedData)
		self.request.close()
		print "Disconnedted from", self.client_address
	


def serve_ftp(PORT):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('', PORT))
	sock.listen(5)

	try:
		while True:		# Main Loop
			newSocket, address = sock.accept()
			print "Connection from", address

			receivedData = newSocket.recv(8192)
			if not receivedData: 
				print 'no data'
				continue

			method = receivedData [ 0 : 4 ].strip() 
			filename = receivedData [ 4 : 132 ].strip()

			print 'the method is %s' % method

			#################################################################
			####		Write LOG Record

			lp = open('pasftpserver.log', 'a')

			lp.write(time.ctime() + ' ' + receivedData [ 0 : 4 ].strip() \
                             + ' ' + receivedData [ 4 : 132 ].strip() + '\n')
			lp.close()


			#################################################################


			if receivedData[ : 132 ].find("HTTP/") != -1:
				# handle HTTP GET request
				print 'recv HTTP Request'
				newSocket.sendall(HTTP_MSG)
				newSocket.close()
				continue
				
				

			if method == 'get':
				try:		# 없는 파일 요청이라면 
					fp = open(filename, 'r')
				except:
					print 'There is not the file'
					newSocket.sendall('There is not the file')
					newSocket.close()
					print "disconnected from", address
					continue
					
				content = fp.read()
				newSocket.sendall( repr( len(content) ) ) # 보낼 길이 전송 
				newSocket.sendall(content) # 내용 전송 
				resp = newSocket.recv(8192) # Client 에서 받은 길이
					
				fp.close()
				newSocket.close()
				print "disconnected from", address
				print "Sent:", filename

			elif method == 'put':
				try: 
					length = int(receivedData [ 132 : 148 ])
					store_dir = receivedData [ 148 : 276 ]
					receivedData = receivedData [ 276 : ]
				except:
					print 'client have no file to put'
					continue

				store_dir = store_dir.strip()
				print store_dir

				if store_dir == 'no_directory_option':
					fp = open(filename, 'w')
				else:					# Directory 옵션을 썼을 때
					try:
						os.listdir(store_dir)
						fp = open(store_dir + filename, 'w')	# 존재하면 그냥 Open
					except:							    # 디렉토리가 존재하지 않으면

						try:
							os.makedirs(store_dir)
						except:							# Permission Deny 경우
							store_dir = '.'

						if store_dir [-1] != '/':
							store_dir += '/'
						fp = open(store_dir + filename, 'w')


				current = len(receivedData) # current:현재까지 기록한 데이터양

				while True:
					if current < length:
						fp.write(receivedData)
						receivedData = newSocket.recv(8192)
						if not receivedData: 
							print 'no data'
							break
						current = current + len(receivedData)
					elif current > length:
						fp.write(receivedData [ : - (current - length) ])
						fp.close()
						break
					elif current == length:
						fp.write(receivedData)
						fp.close()
						break

				newSocket.sendall(repr(length))
				newSocket.close()
				print "disconnected from", address
				print "Saved:", filename

	finally:
		sock.close()

# This is not used
def serve_it(PORT):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('', PORT))
	sock.listen(5)

	try:
		while True:
			newSocket, address = sock.accept()
			print "Connection from", address

			
			flag = False
			current = 0

			while True:
				receivedData = newSocket.recv(8192)
				if not receivedData: 
					print 'no data'
					break

				if flag is False:
					filename = receivedData [ 0 : 32 ].strip()
					length = int(receivedData [ 32 : 48 ])
					receivedData = receivedData [ 48 : ]
					flag = True
					fp = open(filename, 'w')

				current = current + len(receivedData)

				if current > length:
					fp.write(receivedData [ : - (current - length) ])
					fp.close()
					break
				elif current == length:
					fp.write(receivedData)
					fp.close()
					break

				fp.write(receivedData)
			newSocket.sendall(repr(length))
			newSocket.close()
			print "disconnected from", address
			print "Saved", filename

	finally:
		sock.close()


if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'Usuage: python pasFtpServer.py PORT'
		sys.exit(0)
	serve_ftp(int(sys.argv[1]))

	# Threaded Server 용
	#srv = SocketServer.ThreadingTCPServer(('', 3333), ftpHandler)
	#srv.serve_forever()


