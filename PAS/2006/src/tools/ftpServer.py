#!/usr/bin/env python
# -*- coding: EUC-KR -*-

# Ftp Server Program

import socket
import SocketServer
import sys

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
				#print '-' * 30
				#print 'R:', receivedData
				#print '-' * 30
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
		print 'Usuage: python ftpServer.py PORT'
		sys.exit(0)
	serve_it(int(sys.argv[1]))

	# Threaded Server 용
	#srv = SocketServer.ThreadingTCPServer(('', 3333), ftpHandler)
	#srv.serve_forever()


