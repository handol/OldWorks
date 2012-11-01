# -*- coding: EUC-KR -*-
#

# 수신중 강제 접속 종료 확률 (0~100 정수)
closingProbability = 1

import select
import socket
import time
import urlparse
import log
import threading
import struct
import random

#Connection: Keep-Alive


# for KUN
REQMSG = \
"""GET %s HTTP/1.1
SPECIAL_INFO:This is testing request for http proxy server
Host: %s
User-Agent: Mozilla/1.22 (compatible; KUN/1.0; KTF5000; CellPhone)
COUNTER: %d
HTTP_PHONE_NUMBER: 82%s
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0
HTTP_DEVICE_INFO:LX:176,LY:220,CL:8
HTTP_DRIVER_INFO:IMG:MSIS|NBMP,SND:MA3|SMAF|MA5
HTTP_PLATFORM_INFO:PNAME:BREW,PVER:V1.2,PID:1002
HTTP_CHANNEL_INFO:CH:A
HTTP_MNC_INFO:04
HTTP_MDN_INFO:%s
Connection: Keep-Alive
Proxy-Connection: Keep-Alive
Accept: */*
Accept-Language: en
Accept-Encoding: deflate

"""

# for ME
#REQMSG = \
#"""GET %s HTTP/1.0
#Host: %s
#User-Agent: Mozilla/1.22 (compatible; MSMB111; KTF4016; CellPhone)
#COUNTER: %d
#HTTP_PHONE_NUMBER: 82%s
#HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:345,NID:24,SID:2180,BASE_LAT:539768,BASE_LONG:1829452
#HTTP_MDN_INFO:%s
#Accept: 
#Accept-Language: kr
#Accept-Encoding: deflate
#
#"""

def pasreqmsg(url, min, count):
	scheme, host, path, params, query, fragment = urlparse.urlparse(url)
	msg = REQMSG % (url, host, count, min, min)
	return msg



class eh(threading.Thread):
	def __init__(self, reactor, runTimeInfo, RequestPerConnection, UrlList, MaxRequests, addr, port, requestSleepTime, MIN, PipelineNum):
		threading.Thread.__init__(self)
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
		self.activeClose = 0
		self.recvDataSize = 0
		self.requestSleepTime = requestSleepTime
		self.recvBodySize = -1

	def __del__(self):
		if self.isConnected:
			self.close()

	def conn(self, addr, port):
		self.MIN = "0100000%04d" % (random.randint(0, 9999))

		if self.isConnected:
			self.close()

		# init socket
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		buf = struct.pack('ii', 0, 0)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, buf)

		# connect
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
			self.close()

		if log.getLogLevel() >= 3:
			print "eh %3d: send data" % (self.fileno)

		if log.getLogLevel() >= 5:
			print data

	def sendRequest(self):
		self.recvPipelineNum = 0

		self.data = ''
		self.requestClock = time.time()
		self.requestCompleted = False
		self.headerContentLength = -1
		self.recvBodySize = -1

		for i in range(0, self.MaxPipelineNum):
			self.nowRequestPerConnection += 1
			self.makeMsg()
			self.send(self.msg)
			if log.getLogLevel() >= 2:
				print "eh %3d: request URL(%s)" % (self.fileno, self.URL)

			#print self.msg

		# 접속 종료 (1%확률)
		if closingProbability > 0:
			if random.randint(1, 100) <= closingProbability :
				time.sleep(random.randint(0, 100)/100.0)
				self.close()
				self.activeClose += 1

	def makeMsg(self):
		self.URL = self.getRandomUrl()
		self.msg = pasreqmsg(self.URL, self.MIN, self.nowRequestPerConnection)

	def getRandomUrl(self):
		urlsLen = len(self.UrlList)
		r = random.randint(0, urlsLen-1)
		return self.UrlList[r]

	def getHeaderContentLength(self):
		# header 가 모두 받아졌는지 확인
		pos = self.data.find("\r\n\r\n", 0)
		if pos < 0 :
			pos = self.data.find("\n\n", 0)
			if pos < 0 :
				return -1

		lowerData = self.data.lower()

		# content-length 가 해더에 여래개 있을경우 마지막 내용을 사용한다
		lastPos = -1
		pos = 0
		while pos >= 0 :
			pos = lowerData.find("content-length", lastPos+1)
			if pos >= 0 :
				lastPos = pos

		if lastPos < 0 :
			return 0		

		posSeparator = lowerData.find(":", lastPos+1)
		if posSeparator < 0 :
			return -1

		# find line end
		posLineEnd = self.data.find("\n", posSeparator+1)

		found = self.data[posSeparator+1:posLineEnd]

		return int(found)

	def getHeaderConnection(self):
		lowerData = self.data.lower()

		target = "connection"
		pos = 0

		while True:
			pos = lowerData.find(target, pos)

			if pos == -1:
				return "Not Found"

			posSeparator = lowerData.find(":", pos+1)
			if pos == -1:
				continue

			# find line end
			posLineEnd = self.data.find("\n", posSeparator+1)

			found = self.data[posSeparator+1:posLineEnd]

			return found.strip()

	def getHeaderProxyConnection(self):
		lowerData = self.data.lower()

		target = "proxy-connection"
		pos = 0

		while True:
			pos = lowerData.find(target, pos)

			if pos == -1:
				return "Not Found"

			posSeparator = lowerData.find(":", pos+1)
			if pos == -1:
				continue

			# find line end
			posLineEnd = self.data.find("\n", posSeparator+1)

			found = self.data[posSeparator+1:posLineEnd]

			return found.strip()

	def getRealContentLength(self):
		delimiter = "\r\n\r\n"
		pos = self.data.find(delimiter)

		if pos >= 0:
			pos += len(delimiter)
			contentLength = len(self.data) - pos
		else:
			delimiter = "\n\n"
			pos = self.data.find(delimiter)

			if pos < 0:
				return -1;

			pos += len(delimiter)
			contentLength = len(self.data) - pos

		#print "eh %d: RealContentLength = %d" % (self.fileno, contentLength)

		return contentLength

	def handle_input(self):
		try:
			if(self.headerContentLength < 0):
				one = self.sock.recv(4 * 1024)
				self.data += one

				# 요청 완료 여부 확인
				self.headerContentLength = self.getHeaderContentLength()
	
				if self.headerContentLength >= 0:
					self.recvBodySize = self.getRealContentLength()

					if log.getLogLevel() >= 4:
						print "eh %3d: Recv Connection = %s" % (self.fileno, self.getHeaderConnection());
						print "eh %3d: Recv Proxy-Connection = %s" % (self.fileno, self.getHeaderProxyConnection());
	
				if log.getLogLevel() >= 4:
					print "eh %3d: Set headerContentLength = %d" % (self.fileno, self.headerContentLength)
			else:
				one = self.sock.recv(128*1024)
				self.recvBodySize += len(one)

			self.recvDataSize += len(one)
			
		except:
			one = ""

		#print "recv body size : %d, self.headerContentLength  : %d" % (self.recvBodySize, self.headerContentLength)

		# for make network delay
		#time.sleep(0.2)


		# 디버깅 정보 표시
		#if log.getLogLevel() >= 3:
		#	print "eh %3d: recv %10d (+%8d) bytes" % (self.fileno, len(self.data), len(one))

		# 접속 종료
		if len(one) == 0:
			if log.getLogLevel() >= 3:
				print "eh %3d: closed by peer" % (self.fileno)
			self.close()

		# 강제 접속 종료 (1% 확률)
		elif closingProbability > 0 :
			if random.randint(1, 100) <= closingProbability :
				self.close()
				self.activeClose += 1

		# 수신 데이터 처리
		# 기다리는 데이터 사이즈 만큼 수신됐을 때
		if 0 <= self.headerContentLength and self.headerContentLength <= self.recvBodySize:
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

			# request delay
			if self.requestSleepTime > 0.0:
				try:
					time.sleep(self.requestSleepTime)
				except:
					pass

			# max request check
			if self.MaxRequests > 0 and self.MaxRequests <= len(self.runTimeInfo):
				if self.isConnected:
					self.close()
				return;

			# next request
			if(self.recvPipelineNum >= self.MaxPipelineNum):
				if self.nowRequestPerConnection >= self.MaxRequestPerConnection:
					if self.isConnected:
						self.close()
				else:
					self.sendRequest()

			else:
				self.data = ''
				self.requestCompleted = False
				self.headerContentLength = -1
				self.recvBodySize = 0
				self.recvDataSize = 0
					

	def close(self):
		if self.isConnected:
			if log.getLogLevel() >= 4:
				print "eh %3d: socket close" % (self.fileno)

			try:
				self.sock.shutdown(socket.SHUT_RDWR)
			except:
				pass

			self.sock.close()
			self.isConnected = False
			del self.sock

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

				if(localTimeout * waitTime >= 60): # timeout 값
					self.countSelectTimeout += 1
					if log.getLogLevel() >= 3:
						print "eh %3d: select timeout" % (self.fileno)

					timeoutFile = file("timeout.log", "a+")
					timeoutString = "eh %3d: reqURL[%s] headerContentLength[%d] recvBodySize[%d]\n" % (self.fileno, self.URL, self.headerContentLength, self.recvBodySize)
					timeoutFile.write(timeoutString)
					timeoutFile.close()

					self.close()
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
					time.sleep(1.0)
				except:
					pass

		if log.getLogLevel() >= 2:
			print "eh %3d stop........" % (self.fileno)

