#!/usr/bin/env python
# -*- coding: EUC-KR -*-

import SocketServer
import threading
import time

def makeHttpResp(body):
	msg = "HTTP/1.0 200 OK\n"
	msg += "Content-type: text/html;charset=euc-kr\n"
	msg += "Content-Length: %d\n" % (len(body))
	msg += "\n"
	msg += body
	return msg

def prn_sockinfo(sock):
	print "fd= ", sock.fileno()
	print "addr= ", sock.getsockname()
	print "peer= ", sock.getpeername()

setCount = 0
svrCount = 0
respMesg = None	
respBodyLeng = 0

# CP RESP 메시지의 header 와  body 길이를 세팅한다.
def setRespMesg(respheader, respbodyleng):
	global respMesg, respBodyLeng, setCount
	respMesg = respheader
	respBodyLeng = respbodyleng
	setCount += 1
	leng = len(respMesg) + respBodyLeng
	print "CpWeb SET resp: %d H=%d, B=%d cnt=%d" % (leng, len(respMesg), respBodyLeng, setCount)

def prnFirstLine(reqmsg):
	pos = reqmsg.find("\r")
	if pos == -1:
		pos = reqmsg.find("\n")
	if pos != -1:
		print "\t",reqmsg[:pos]
	else:
		print "\t",reqmsg
		
# RequestHandler Objects 
# 아래의 3가지 함수를 정의할 수 있다.
# setup()  -- handle() 전에 호출된다.
#  handle()
# finish() --  handle() 후에 호출된다.

class MyHandler(SocketServer.BaseRequestHandler):
	def stop(self):
		self.runFlag = False

	def setup(self):
		self.runFlag = True
		
	def handle(self):
		global respMesg		

		self.debug = False
		
		if False:
			prn_sockinfo(self.request) # socket
		print "CpWeb: %s" % (self)
		print "CpWeb: Client from", self.client_address # client 주소

		while self.runFlag:
			data = self.request.recv(1024)
			print 'CpWeb: Read %d bytes [%d]' % (len(data), self.request.fileno() )
			if self.debug and len(data) > 0: print data
			
			
			
			if len(data) > 0 and respMesg != None:
				prnFirstLine(data)
				dataToSend = respMesg + '+' * respBodyLeng
				self.request.send(dataToSend)
				print 'CpWeb: Sent %d bytes (H=%d, B=%d) [%d]' % \
				 (len(dataToSend), len(respMesg), respBodyLeng, self.request.fileno() )
				prnFirstLine(respMesg)
				if self.debug: print respMesg
				time.sleep(0.5)
				return
			
			if not data: 
				# means peer closed
				print "CpWeb: peer closed"
				break

class MyTcpServer(SocketServer.ThreadingTCPServer):
	def __init__(self, svraddr, handlerClassName):
		SocketServer.ThreadingTCPServer.__init__(self, svraddr, handlerClassName)
		self.runFlag = True
	
	def stop(self):
		self.runFalg = False

	# serve_forever( ) 
	# Handle an infinite number of requests. This simply calls handle_request() inside an infinite loop. 
	def server_forever(self):
		print "MyTcpServer Starting ..."
		while self.runFlag:
			self.handle_request()
		print "MyTcpServer stopped sock[%d]" % (self.fileno())

server = None
t = None

def	runServer(port):
	global server, t
	server = MyTcpServer( ('', port), MyHandler) 
	
	#t = threading.Thread(target=server.server_forever, args=(None,))
	t = threading.Thread(target=server.server_forever)
	print "CpWeb: MyServer Running ... port=%d" % (port)
	t.start()
	
def	stopServer():
	global server, t
	server.stop()
	t.join()
	
# SocketServer 를 별도의 쓰레드로 기동하기 위하여.
class MyServer(threading.Thread):
	def __init__(self, port, debug=0):
		global respMesg	
		threading.Thread.__init__(self)
		self.port = port
		respMesg = "Hello there !"

	def run(self):
		self.myServer = SocketServer.ThreadingTCPServer(('', self.port), MyHandler)
		print "CpWeb: MyServer Running ..."
		self.myServer.serve_forever( )
		print "CpWeb: MyServer Stopped"
		
	
	def stop(self):
		print "CpWeb: MyServer Stopping ..."
		self.myServer.server_close()
		
if __name__=='__main__':
	import sys
	import time

	if len(sys.argv) >= 2:
		port = int(sys.argv[1])
	else:
		print "Usage: port_num"
		sys.exit(0)

	respMesg = "Hello there !"
	setRespMesg("Hello there !!!!!!", 0)
	
	#myServer = SocketServer.TCPServer(('',8080), MyHandler)
	
	#myServer = SocketServer.ThreadingTCPServer(('',port), MyHandler)
	#myServer.serve_forever( )
	runServer(port)
	
	cnt = 0	
	while 1:
		# 사용자가 CTRL-C를 누른 경우 멈추기 위해 try-except 구문 사용.
		try:
			time.sleep(1)
			print time.strftime("%H:%M:%S")
			cnt += 1			
		except:
			#myServer.stop()
			stopServer()
			break
