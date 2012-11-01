#!/usr/bin/env python
# -*- coding: EUC-KR -*-

# DNS Query Program

import socket
import SocketServer
import sys
import time
import threading
import thread

DNS_table = {}
Wait_time = 0.8

class DNSHandler(SocketServer.BaseRequestHandler):
	''' Threaded Server 구현을 위해서 '''

	def handle(self):
		print "Connected from", self.client_address
		#while True:
		receivedData = self.request.recv(8192)
			#if not receivedData: break

			#self.request.sendall(receivedData)

		receivedData = receivedData.strip()
		t = threading.Thread(target=self.getIP, args=(receivedData,))

			#try:

		self.hostIP = '0.0.0.0'				# 1초 넘으면 0.0.0.0 return
		t.start()
		t.join(Wait_time)					# Timeout 만큼 기다린다.
		print '    ', receivedData, ';', self.hostIP

		if self.hostIP == '0.0.0.0' and DNS_table.has_key(receivedData) is True:
			self.hostIP = DNS_table.get(receivedData)

		self.request.sendall(self.hostIP)		# SEND

		if self.hostIP != '0.0.0.0':
			DNS_table [ receivedData ] = self.hostIP	# 기존 데이터 업데이트 

			#except: 
				#print 'Invalid Domain Request'
				#newSocket.sendall('0.0.0.0')

		self.request.close()
		print DNS_table
		print "Disconnected from", self.client_address
		print '-' * 80

	def getIP(self, Domain):
		#time.sleep(1.5)
		self.hostIP = socket.gethostbyname(Domain)



def getsIP(Domain):
	''' domain 을 받아 IP를 돌려준다. '''
	global hostIP
	time.sleep(1.1)
	hostIP = socket.gethostbyname(Domain)

def serve_it(PORT):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.bind(('', PORT))
	sock.listen(5)
	global hostIP

	while True:
		newSocket, address = sock.accept()
		print "Connection from", address
		
		#while True:
		receivedData = newSocket.recv(8192)
		print "Request Domain:", receivedData
			#if not receivedData: 
				#print 'no data'
				#break
		

		receivedData = receivedData.strip()
		t = threading.Thread(target=getsIP, args=(receivedData,))

		#try:

		hostIP = '0.0.0.0'
		t.start()
		t.join(0.8)
		print hostIP
		newSocket.sendall(hostIP)

		#except: 
			#print 'Invalid Domain Request'
			#newSocket.sendall('0.0.0.0')


		newSocket.close()
		print "disconnected from", address

	sock.close()

if __name__ == '__main__':
	if len(sys.argv) != 3:
		print 'Usuage: python DnsCache.py PORT WAIT_TIME(DEFAULT=0.8s)'
		sys.exit(0)
	#serve_it(int(sys.argv[1]))

	# Threaded Server 용

	Wait_time = float(sys.argv[2])
	srv = SocketServer.ThreadingTCPServer(('', int(sys.argv[1])), DNSHandler)
	srv.serve_forever()


