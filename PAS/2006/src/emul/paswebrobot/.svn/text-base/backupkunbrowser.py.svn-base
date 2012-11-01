#!/usr/bin/env python
# -*- coding: EUC-KR -*-

################################################################################
########################### PAS client: Browser ################################
################################################################################

import socket
import time
import threading
import select

REQMSG = \
"""GET %s HTTP/1.1
Host: 
User-Agent: Mozilla/1.22 (compatible; PDAKUN/1.0; KTF5000; CellPhone)
COUNTER:1
HTTP_PHONE_NUMBER: 82
HTTP_PHONE_SYSTEM_PARAMETER: BASE_ID:326, NID:36, SID:2189, BASE_LAT:0, BASE_LONG:0
HTTP_DEVICE_INFO:LX:176,LY:220,CL:8
HTTP_DRIVER_INFO:IMG:MSIS|NBMP,SND:MA3|SMAF|MA5
HTTP_PLATFORM_INFO:PNAME:BREW,PVER:V1.2,PID:1002
HTTP_CHANNEL_INFO:CH:A
HTTP_MNC_INFO:04
HTTP_MDN_INFO:
Cookie: SITESERVER=ID=958c5b3e4ce473494a6db82d70887570;
Proxy-Connection: Keep-Alive
Accept: */*
Accept-Language: en
Accept-Encoding: deflate
\r
\r
"""

################################################################################
########################### Class Header #######################################
################################################################################
class Header(dict):
        "Header build, parse, add, replace and so on"
        def __init__(self):
		self.MSG = {}
		self.FirstHeader = "GET %s HTTP/1.1\n"
		self.OutMsg = ""

        def build(self, filename):			# Build String Header
		self.OutMsg = ""
		self.OutMsg += self.FirstHeader
		for i in self.MSG.keys():
			self.OutMsg = self.OutMsg + i + ': ' + self.MSG.get(i) 
			self.OutMsg += '\n'
		self.OutMsg += "\r\n\r\n"
		return self.OutMsg % filename

        def parse(self, msg):
		self.MSG = {}
                lines = msg.split('\n')
                #lines = lines[1:17]    		# we should fix
                for line in lines[ 1 : ]:
			INX = line.find(':')
			if INX == -1:
				break
			_name = line[0 : INX]
			_value = line[INX+1 : ]
			self.MSG[ _name.strip() ] = _value.strip()

        def add(self, name, value):
                self.MSG[name] = value

        def addIfNot(self, name, value):
                if self.MSG.has_key(name):
                        print "already exist that name"
                else:
                        self.MSG[name] = value
                        
        def getValue(self, name):
                return self.MSG.get(name, 0) # if not exist, return 0

        def length(self):
                return len(self.MSG)

        def replace(self, name, value):
                if self.MSG.has_key(name):
                        self.MSG[name] = value
                else:
                        print "can't replace: such name doesn't exist"

################################################################################
########################### Class RecHeader Inherit Header #####################
################################################################################
class RecHeader(Header):

	def __init__(self):
		Header.__init__(self)
		self.code = ""				# 200, 302 STATUS code
		self.body = ""				# HTML BODY

	def parse(self, msg):
		self.MSG = {}
		inx = msg.find("\r\n\r\n")		# split head, body - begin
		msg_head = msg[ : inx]
		self.body = msg[inx+4 : ]

		if inx == -1:
			inx = msg.find("\n\n")
			msg_head = msg[ : inx]
			self.body = msg[inx+2 : ]

		assert inx != -1, "inx error"		# DEBUG
							# split head, body - end
		lines = msg_head.split('\n')
		self.code = lines[0].split()[1]
		for line in lines[ 1 : ]:
			INX = line.find(':')
			_name = line[0 : INX]
			_value = line[INX+1 : ]
			self.MSG[ _name.strip() ] = _value.strip()
	
	def getStatusCode(self):
		return self.code

	def getBody(self):
		return self.body


################################################################################
########################### Class KunClient ######################################
################################################################################
class KunClient(threading.Thread):

	def __init__(self, host='localhost', port=50007, debug=0):
		threading.Thread.__init__(self)
		self.header = Header()						# send Header
		self.recheader = RecHeader()					# received Header
		self.host = host
		self.port = port
		self.debug = debug
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		
	def run(self):
		time.sleep(5)
		print "runned..", threading.currentThread()

	def conn(self):
		self.sock.setblocking(1)
		if self.debug: print "after socket(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])

		#s.bind(("192.168.2.6", port))
		#s.bind((host, port))

		if self.debug: print "after bind(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])

		#try: 
		self.sock.connect((self.host, self.port))
		#except:
			#print 'connection failed !!'

		if self.debug: print "after connect(): fd=%d, addr=%s, %d" % (self.sock.fileno(), self.sock.getsockname()[0], self.sock.getsockname()[1])

		if self.debug: print "peer name: %s %d" % (self.sock.getpeername()[0], self.sock.getpeername()[1])

		return 1	# return 1 if success

	def sendmsg(self, msg):
		self.sock.send(msg)
	
		start = time.time()
		data = ''
		while len(data) == 0:
			time.sleep(0.2)
			time.sleep(0.2)
			try:
				one = self.sock.recv(1024)
				#if not one: break
				#if len(one) < 1: break
				data += one
			except:
				continue	
		
		print data
		end = time.time()
		print "LENGTH=%d" % (len(data))
		print "lap time = %d" % (end - start)
		return data

	def handleRespond(self, result):
		pass
		
	def close(self):
		self.sock.close()
	

	
################################################################################

if __name__ == "__main__":
	import sys
	import urlparse

	HOST = 'ktfkunproxy.ktf.com'    # The remote host
	HOST = '210.123.92.140'
	HOST = '172.23.35.87'
	HOST = 'localhost'    # The remote host

	PORT = 50007              # The same port as used by the server
	PORT = 8080

	MDN = "820161002000"  ## Virtual MDN
	MDN = "8201690102000"
	MDN = "8201690102000"
	MDN = "0162010022"   	################################ - 82 ###
	URL = "http://www.magicn.com한돌"
	URL = "http://www.magicn.com/oO???oc?UA????????UA???U®?????eø??Oⓒ???|º??|"
	URL = "http://www.magicn.com/"

	if len(sys.argv)==2:
		if sys.argv[1]=='good':
			MDN = "820162010022"
			URL = "http://www.magicn.com/"
		elif sys.argv[1]=='santa':
			MDN = "8201690102000"
			URL = "http://www.magicn.com/"
		elif sys.argv[1]=='url':
			MDN = "820162010022"
			URL = "http://www.magicn.com/oO???oc?UA????????UA???U®?????eø??Oⓒ???|º??|"
		elif sys.argv[1]=='help' or sys.argv[1]=='?':
			print "usage: port mdn url"
			print "mdn = 820161002000, 8201690102000"
			sys.exit()

	if len(sys.argv) >= 4:
		PORT = int(sys.argv[1])
		MDN = sys.argv[2]
		URL = sys.argv[3]

	scheme, host, path, params, query, fragment =\
		urlparse.urlparse(URL)

	#print "## Paras: ", HOST, URL, host, MDN
	#msg = REQMSG % (URL, host, MDN, MDN)	

###########################################################################

	print "------------------------------------------------------------------"
	print " KunBrowser Started.. "
	print " HOST = %s PORT = %s MSN = %s " % (HOST, PORT, MDN)
	print "------------------------------------------------------------------"


	client = KunClient(host=HOST, port=PORT) 				# make instance
	client.conn() 							# conn Method return 1, if success

	client.header.parse(REQMSG)					# make initial Head 0.9
	client.header.add("HTTP_PHONE_NUMBER", MDN)
	client.header.add("HTTP_MDN_INFO", MDN)
	client.header.add("Host", host)

	while 1:
		CMD = raw_input(" Z:\<KunBrowser> ")
		if CMD == "go":

			if client.recheader.getStatusCode() == "":			# IF First LOOP (Execution)
				message = client.header.build('http://magicn.com') 	# make initial Head 1.0
				result = client.sendmsg(message)			# Send Header
				client.recheader.parse(result)				# Parsing Received Head, Body

			elif client.recheader.getStatusCode() == "302":			# IF HTTP 302 Response
				scheme, host, path, params, query, fragment = \
					urlparse.urlparse(client.recheader.getValue('Location'))
				client.header.replace("Host", host)

				message = client.header.build(client.recheader.getValue('Location'))	# GET by New Location

				result = client.sendmsg(message)
				client.recheader.parse(result)				# Parsing Received Head, Body

########
####### Some comment to remember 
####### header - will send - after once parse() - slow change value
######  recheader - received - after several parse() - ALL change
######  setcookie  module setting  :cookielib
######


			#print 'recheader test -----------------'
			#print client.recheader.MSG
			#print client.recheader.getBody()
			#print client.recheader.getStatusCode()


		else: break
		
	client.close() # socket close Method
	print '''

	 #####    #   #  ######
	 #    #    # #   #
	 #####      #    #####   #####           #####
	 #    #     #    #
	 #    #     #    #
	 #####      #    ######         #######
	 '''


	#print "------------------------------------------------------------------"
	#print " KunBrowser Ended..^_^^_^^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^_^"
	#print "------------------------------------------------------------------"


	############# Test 1 : for KunClient class ##########################

	#client = []
	#for i in range(1027):
		#client.append( KunClient(host=HOST, port=PORT) )
	#else: print i
	
	#print "starting..."
	#for i in range(1027):
		#client[i].start()

	###################################################################


	############# Test 2 : for Header class ###########################

        #hoho = Header()
        #hoho.parse(REQMSG)
        #hoho.add('Accept','*/*')
	##assert hoho.MSG
	#print hoho.MSG
        #print hoho.getValue('Cookie')
        #print hoho.length()
        #hoho.replace('Host', 'brushzzang')
        #print hoho.build()
	
	##################################################################


