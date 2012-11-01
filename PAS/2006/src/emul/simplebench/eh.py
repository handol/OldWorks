# -*- coding: EUC-KR -*-
#

import select
import socket
import time
import urlparse
import random
import log
import threading
import struct

#Connection: Keep-Alive

random.seed(time.time())


REQMSG = \
"""GET %s HTTP/1.1
Host: %s
User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
COUNTER:1
HTTP_PHONE_NUMBER: 82%s
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0
HTTP_DEVICE_INFO:LX:176,LY:220,CL:8
HTTP_DRIVER_INFO:IMG:MSIS|NBMP,SND:MA3|SMAF|MA5
HTTP_PLATFORM_INFO:PNAME:BREW,PVER:V1.2,PID:1002
HTTP_CHANNEL_INFO:CH:A
HTTP_MNC_INFO:04
HTTP_MDN_INFO:%s
Cookie: SITESERVER=ID=958c5b3e4ce473494a6db82d70887570;
Proxy-Connection: Keep-Alive
Accept: */*
Accept-Language: en
Accept-Encoding: deflate\r\n\r\n"""

def pasreqmsg(url, min):
	scheme, host, path, params, query, fragment = urlparse.urlparse(url)
	#msg = REQMSG % (path, host, min)
	msg = REQMSG % (url, host, min, min)
	return msg



class eh(threading.Thread):
	def __init__(self, reactor, runTimeInfo, RequestPerConnection, UrlList, MaxRequests, addr, port, requestSleepTime, MIN, PipelineNum):
		threading.Thread.__init__(self)
		self.urlptr = 0
		self.reactor = reactor
		self.runTimeInfo = runTimeInfo
		self.isConnected = False
		self.requestCompleted = True
		self.requestClock = 0.0
		self.data = ''
		self.headerContentLength = -1
		self.URL = "http://172.23.35.87"
		self.MIN = MIN
		self.MaxRequestPerConnection = RequestPerConnection
		self.nowRequestPerConnection = 0
		self.MaxPipelineNum = PipelineNum
		self.recvPipelineNum = 0
		self.UrlList = UrlList
		self.fileno = -1
		self.MaxRequests = MaxRequests
		self.addr = addr
		self.port = port
		self.countConnectionSuccess = 0
		self.countConnectionFail = 0
		self.countSelectTimeout = 0
		self.recvDataSize = 0
		self.requestSleepTime = requestSleepTime
		self.recvBodySize = -1

	def __del__(self):
		if self.isConnected:
			self.sock.close()
			del self.sock


	def conn(self, addr, port):
		if self.isConnected:
			self.close()

		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

		buf = struct.pack('ii', 0, 0)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, buf)

		try:
			self.sock.connect( (addr, port) )

			self.fileno = self.sock.fileno()
			self.connectedAddr = addr
			self.connectedPort = port
			self.isConnected = True
			self.nowRequestPerConnection = 0
			self.countConnectionSuccess += 1
			self.data = ''
			if log.getLogLevel() >= 3:
				print "eh %3d: connected" % (self.fileno)
		except:
			self.isConnected = False
			self.countConnectionFail += 1
			if log.getLogLevel() >= 1:
				print "eh %3d: connection fail" % (self.fileno)

	def send(self, data):
		try:
			self.sock.send(data)
		except:
			close()
		#print "eh %d: send" % (self.fileno)

	def sendRequest(self):
		if self.requestSleepTime > 0.0:
			try:
				time.sleep(self.requestSleepTime)
			except:
				pass

		self.recvPipelineNum = 0

		self.data = ''
		self.requestClock = time.time()
		self.requestCompleted = False
		self.headerContentLength = -1
		self.nowRequestPerConnection += 1
		self.recvBodySize = -1

		for i in range(0, self.MaxPipelineNum):
			self.makeMsg()
			self.send(self.msg)
			if log.getLogLevel() >= 2:
				print "eh %3d: request URL(%s)" % (self.fileno, self.URL)

			#print self.msg

	def makeMsg(self):
		self.URL = self.getRandomUrl()
		self.msg = pasreqmsg(self.URL, self.MIN)

	def getRandomUrl(self):
		#self.urlptr += 1
		#self.urlptr %= len(self.UrlList)
		#r = self.urlptr

		urlsLen = len(self.UrlList)
		r = random.randint(0, urlsLen-1)
		return self.UrlList[r]

	def getHeaderContentLength(self):
		target = "content-length:"
		lowerData = self.data.lower()
		pos = lowerData.find(target)

		if pos == -1:
			return -1

		pos += len(target)
		pos2 = self.data.find("\n", pos)
		found = self.data[pos:pos2]
		return int(found)

	def getRealContentLength(self):
		delimiter = "\r\n\r\n"
		pos = self.data.find(delimiter)

		if pos < 0:
			return -1

		pos += len(delimiter)
		contentLength = len(self.data) - pos

		#print "eh %d: RealContentLength = %d" % (self.fileno, contentLength)

		return contentLength

	def handle_input(self):
		try:
			if(self.headerContentLength < 0):
				one = self.sock.recv(1024)
				self.data += one
			else:
				one = self.sock.recv(self.headerContentLength - self.recvBodySize)
				self.recvBodySize += len(one)
		except:
			one = ""

		#print "recv body size : %d, self.headerContentLength  : %d" % (self.recvBodySize, self.headerContentLength)

		self.recvDataSize += len(one)

		# 디버깅 정보 표시
		if log.getLogLevel() >= 3:
			print "eh %3d: recv %10d (+%8d) bytes" % (self.fileno, len(self.data), len(one))


		# 디버깅 정보 표시
		if log.getLogLevel() >= 5:
			if len(one) > 0:
				print "%s" % (one)

		# 접속 종료
		if len(one) == 0:
			if log.getLogLevel() >= 3:
				print "eh %3d: closed by peer" % (self.fileno)
			self.close()

		else:
			# 요청 완료 여부 확인
			if self.headerContentLength < 0:
				self.headerContentLength = self.getHeaderContentLength()
	
				if self.headerContentLength > 0:
					self.recvBodySize = self.getRealContentLength()
	
				if log.getLogLevel() >= 4:
					print "eh %3d: Set headerContentLength = %d" % (self.fileno, self.headerContentLength)


			if(self.headerContentLength < self.recvBodySize):
				print "-.-;"	

			

			# 기다리는 데이터 사이즈 만큼 수신됐을 때
			if self.recvBodySize >= 0 and self.headerContentLength <= self.recvBodySize:
				self.requestCompleted = True
				endTime = time.time()
				elapsedTime = endTime - self.requestClock

				if log.getLogLevel() >= 1:
					print "eh %3d: request complete. elapsed time %f" % (self.fileno, elapsedTime)

				if self.headerContentLength < self.recvBodySize:
					print "self.headerContentLength[%d] < self.recvBodySize[%d]" % (self.headerContentLength, self.recvBodySize)

				info = [self.URL, elapsedTime, self.recvBodySize, self.requestClock, endTime]
				self.runTimeInfo.append(info)

				self.recvPipelineNum += 1

				if(self.recvPipelineNum >= self.MaxPipelineNum):
					if self.nowRequestPerConnection >= self.MaxRequestPerConnection:
						if self.isConnected:
							self.closeRequest()

					elif self.MaxRequests > 0 and self.MaxRequests <= len(self.runTimeInfo):
						if self.isConnected:
							self.closeRequest()

					else:
						self.sendRequest()
				else:
					self.data = ''
					self.requestCompleted = False
					self.headerContentLength = -1
					self.recvBodySize = -1
					

	def close(self):
		try:
			self.sock.shutdown(socket.SHUT_RDWR)
		except:
			pass

		self.sock.close()
		del self.sock
		self.isConnected = False

	def closeRequest(self):
		if log.getLogLevel() >= 3:
			print "eh %3d: reqeust close" % (self.fileno)

		if self.isConnected == False:
			print "eh %3d: already closed" % (self.fileno)

		self.sock.shutdown(socket.SHUT_WR)

	def proc(self):
		localTimeout = 0
		waitTime = 1

		while self.isConnected and self.isRun:
			handles = [self.sock.fileno()]
			try:
				reads, writes, in_erros = select.select(handles, [], [], waitTime)
			except:
				reads = []

			for sock in reads:
				#print "reactor: event on sock %d" % (sock.fileno())
				self.handle_input()

			if len(reads) == 0:
				localTimeout += 1
				if(localTimeout * waitTime >= 30): # timeout 값
					self.countSelectTimeout += 1
					if log.getLogLevel() >= 3:
						print "eh %3d: select timeout" % (self.fileno)
					self.closeRequest()
					break
			else:
				localTimeout = 0

	def run(self):
		self.isRun = True

		if log.getLogLevel() >= 2:
			print "eh %3d: start......." % (self.fileno)

		while (self.MaxRequests <= 0 or self.MaxRequests > len(self.runTimeInfo)) and self.isRun:
			self.conn(self.addr, self.port)

			if self.isConnected :
				self.sendRequest()
				self.proc()
			else:
				try:
					time.sleep(0.01)
				except:
					pass

		if log.getLogLevel() >= 2:
			print "eh %3d stop........" % (self.fileno)

