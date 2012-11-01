#!/usr/bin/env python
# -*- coding: EUC-KR -*-

import select
import socket
import time
import threading
import thread
import struct

def prnFirstLine(reqmsg):
        pos = reqmsg.find("\r")
	if pos == -1:
		pos = reqmsg.find("\n") 
	if pos != -1:
		print "\t",reqmsg[:pos]
	else:
		print "\t",reqmsg

class TcpClient(threading.Thread):
	def __init__(self, debug=0):
		threading.Thread.__init__(self)
		self.isConnected = False		
		self.data = ''
		self.ContentLength = -1
		self.URL = "http://172.23.35.87"
		self.debug = debug
		
		self.recvDataSize = 0
		self.isStop = False
		self.waitRes = False
		# thread, lock usage: http://linuxgazette.net/107/pai.html
		self.lock = thread.allocate_lock()


	def __del__(self):
		if self.isConnected:
			self.sock.close()
			del self.sock
	
	def stop(self):
		print "Phone: Stopping... sock[%d]" % (self.sock.fileno())
		self.isStop = True
		self.sock.close()
		print "Phone:", self.sock, self.isStop
		
	def conn(self, addr, port):
		if self.isConnected:
			self.sock.close()
			
		self.addr = addr
		self.port = port
		
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)		

		#print "sock=", self.sock
		print "Phone: sock fd=%d created" % ( self.sock.fileno() )
		#buf = struct.pack('ii', 0, 0)
		#self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, buf)

		print "Phone: Connecting %s %d" % (self.addr, self.port)
		try:
			self.sock.connect( (self.addr, self.port) )
			self.isConnected = True			
			self.data = ''			
			print "Phone: sock %d connected" % (self.sock.fileno())
		except:
			self.isConnected = False			
			print "Phone: sock %d connection fail" % (self.sock.fileno())
		#self.sock.setblocking(0) # set non-blocking socket

	def send(self, data):
		self.waitRes = True
		self.data = ''
		self.recvDataSize = 0 
		try:
			print "Phone: sent %d bytes [%d]" % ((len(data)), self.sock.fileno())
			prnFirstLine(data)
			self.sock.send(data)			
			if self.debug:
				print data
			return 0	
		except:
			print "Phone: send failed. sock[%d]" % (self.sock.fileno())
			self.stop()
			time.sleep(0.1)
			return -1
		#print "eh %d: send" % (self.sock.fileno())
	
	def proc(self):

		waitTime = 1.0
		try:
			if self.isStop: return -1
			reads, writes, in_erros = select.select([self.sock], [], [], waitTime)
		except:
			#print "Phone: ERROR in select() sock[%d]" % (self.sock.fileno())
			print "Phone: ERROR in select()"
			return
			raise
			reads = []
			
		if reads:		
			while True:
				res = self.handle_input()
				if res < 0: return -1
				if res == 0: break
				#time.sleep(0.01)
				break

			return 0
		else:
			if self.debug:
				print "."
				print reads
				
			return 0
			
	def handle_input(self):
		try:
			one = self.sock.recv(1024*64)
			self.data += one
			self.recvDataSize += len(one)
			if len(one) > 0:
				self.waitRes = False
				#print "Phone: recv %d total %d" % (len(one), self.recvDataSize)
				print "Phone: recv %d sock[%d]" % (len(one), self.sock.fileno())
				if self.debug:						
					print one
				return 1
			return 0

		except:
			return -1
	
	
	# thread 클래스는 start()라는 메쏘드에서 쓰레드가 기동되며, start()는 run() 메쏘드를 호출한다.
	# run() 메쏘드를 우리 입맛에 맞게 아래와 같이 정의한다. 일반적으로 루프를 포함하여야 한다.
	def run(self):
		print "TcpClient run() starts"
		while not self.isStop:			
			if self.proc() < 0:
				break
		print "TcpClient stops"


#----------------------------------------------------------------------------
if __name__ == "__main__":
	import sys
	import datetime
	
	if len(sys.argv) < 3:
		print "Usage: server_addr server_port"
		sys.exit(0)
	cli = TcpClient()
	cli.conn(sys.argv[1], int(sys.argv[2]))
	cli.start()
	#time.sleep(0.1)
	cli.send("GET http://localhost HTTP/1.0\r\n\r\n")
	
	cnt = 0
	
	while 1:
		# 사용자가 CTRL-C를 누른 경우 멈추기 위해 try-except 구문 사용.
		try:
			time.sleep(1)
			print time.strftime("%H:%M:%S")
			cnt += 1
			if cnt % 3 == 0:
				if cli.send("GET http://localhost HTTP/1.0\r\n\r\n") < 0:
					break
		except:
			cli.stop()
			break
	#cli.send("GET / HTTP/1.1\r\n\r\n")
