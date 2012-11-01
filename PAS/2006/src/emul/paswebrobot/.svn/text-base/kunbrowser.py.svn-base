#!/usr/bin/env python
# -*- coding: EUC-KR -*-

################################################################################
###########################  Kun Browser 
################################################################################

import socket
import time
import threading
import select

import urllib2
import cookielib
import sys

import hparser

REQMSG = \
"""GET %s HTTP/1.1
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
Proxy-Connection: Keep-Alive
Accept: */*
Accept-Language: en
Accept-Encoding: deflate
\r
\r
"""
#Cookie: SITESERVER=ID=958c5b3e4ce473494a6db82d70887570;     cookie handler don't like this
#Host: 

################################################################################
# Inherit from urllib2 for handling 302, log ...
################################################################################

class MyHTTPRedirectHandler(urllib2.HTTPRedirectHandler):
	def __init__(self, fp):
		self.fp = fp
		#urllib2.HTTPRedirectHandler.__init__(self)
	
	def http_error_302(self, req, fp, code, msg, headers):
		#print "it works well 302 handler"
		self.fp.write(time.asctime()[ 4 : ] + ' ')

		#fp.write(repr(seq_num) + ' ')
		self.fp.write('0 ')
		#seq_num = seq_num + 1
		#self.fp.write(repr(depth) + ' ')
		self.fp.write('0' + ' ')

		#self.fp.write('HEAD' + '/0 ')
		self.length = 0
		for i in req.unredirected_hdrs:
			self.length = self.length + len(i) + len(req.unredirected_hdrs[i])
		self.fp.write(repr(len(req.headers.__str__()) + self.length+6) + '/0 ')

		self.fp.write(repr(len(headers.__str__())) + '/0 ')

		self.fp.write(repr(code) + ' ')
		self.fp.write('0.0 ')
		self.fp.write(req.get_full_url())

		#fp.write('%.3s' % (end - start) + ' ')
		self.fp.write('\n')

		#print dir(req)
		#print '-' * 55
		#print req.has_header('Cookie')
		#print req.has_header('Accept')
		#print req.headers
		
		#self.ttt = ['_Request__original', '_Request__r_host', '_Request__r_type', '__doc__', '__getattr__', '__init__', '__module__', 'add_data', 'add_header', 'add_unredirected_header', 'data', 'get_data', 'get_full_url', 'get_header', 'get_host', 'get_method', 'get_origin_req_host', 'get_selector', 'get_type', 'has_data', 'has_header','header_items', 'headers', 'host', 'is_unverifiable', 'origin_req_host', 'port', 'set_proxy', 'type', 'unredirected_hdrs', 'unverifiable']
		#print req.readheaders()

		return urllib2.HTTPRedirectHandler.http_error_302(self, req, fp, code, msg, headers)

class MyHTTPHandler(urllib2.HTTPHandler):
	def http_open(self, req):
		#print "$" * 50
		return urllib2.HTTPHandler.http_open(self, req)

	
###############################################################################r
########################### Class Header #######################################
################################################################################
class Header(dict):
        "Header build, parse, add, replace and so on"
        def __init__(self):
		self.header_table = {}
		self.GetHeader = "GET %s HTTP/1.1\n"

        def build(self, filename):			# Build String Header
		self.OutMsg = ""
		self.OutMsg += self.GetHeader
		for i in self.header_table.keys():
			self.OutMsg = self.OutMsg + i + ': ' + self.header_table.get(i) 
			self.OutMsg += '\n'
		self.OutMsg += "\r\n\r\n"
		return self.OutMsg % filename

        def parse(self, msg):
		self.header_table = {}
                lines = msg.split('\n')
                #lines = lines[1:17]    		# we should fix
                for line in lines[ 1 : ]:
			INX = line.find(':')
			if INX == -1:
				break
			_name = line[0 : INX]
			_value = line[INX+1 : ]
			self.header_table[ _name.strip() ] = _value.strip()

        def add(self, name, value):
                self.header_table[name] = value

        def addIfNot(self, name, value):
                if self.header_table.has_key(name):
                        print "already exist that name"
                else:
                        self.header_table[name] = value
                        
        def getValue(self, name):
                return self.header_table.get(name, 0) # if not exist, return 0

        def length(self):
                return len(self.header_table)

        def replace(self, name, value):
                if self.header_table.has_key(name):
                        self.header_table[name] = value
                else:
                        print "can't replace: such name doesn't exist"

################################################################################
########################### Class RecHeader Inherit from Header ################
########################### THIS IS TEST VERSION
################################################################################
class RecHeader(Header):

	def __init__(self):
		Header.__init__(self)
		self.code = ""				# 200, 302 STATUS code
		self.body = ""				# HTML BODY

	def parse(self, msg):
		self.header_table = {}
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
			self.header_table[ _name.strip() ] = _value.strip()
	
	def getStatusCode(self):
		return self.code

	def getBody(self):
		return self.body


################################################################################
########################### Class KunClient ####################################
########################### THIS IS TEST VERSION
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
	

class KunEngine(threading.Thread):
	'''	Main Class of KunBrowser
		
		You just make instance of this class, and execute do_request
	'''

	def __init__(self):
		threading.Thread.__init__(self)
		self.header = Header()
		self.header.parse(REQMSG)

		self.cj = cookielib.CookieJar()
		cookie_handler = urllib2.HTTPCookieProcessor(self.cj)
		#proxy_support = urllib2.ProxyHandler({'http':'http://localhost:8080'})
		self.proxy_support = urllib2.ProxyHandler({'http':'http://127.0.0.1:7090'})
		self.http_handler = urllib2.HTTPHandler(debuglevel = 0)	# DEBUG SHOW OPTION 

		self.handlers = [ self.http_handler, self.proxy_support, self.cookie_handler ]

		#opener = urllib2.build_opener(handler, proxy_support, urllib2.HTTPCookieProcessor(cj))
		self.opener = urllib2.build_opener(*self.handlers)
		urllib2.install_opener(self.opener)
		
	def set_debug(self):
		self.debug = 1

	def unset_debug(self):
		self.debug = 0

	def set_header(self, attr, value):
		self.header.add(attr, value)

	def do_request(self, url):
		pass

################################################################################

if __name__ == "__main__":
	import sys
	import urlparse

	#HOST = 'ktfkunproxy.ktf.com'    # The remote host
	#HOST = '210.123.92.140'
	#HOST = '172.23.35.87'

	#PORT = 50007              # The same port as used by the server

	#MDN = "820161002000"  ## Virtual MDN
	#MDN = "8201690102000"
	#MDN = "8201690102000"
	#URL = "http://www.magicn.com한돌"
	#URL = "http://www.magicn.com/oO???oc?UA????????UA???U®?????eø??Oⓒ???|º??|"
	#URL = "http://www.magicn.com/"
#
#	if len(sys.argv)==2:
#		if sys.argv[1]=='good':
#			MDN = "820162010022"
#			URL = "http://www.magicn.com/"
#		elif sys.argv[1]=='santa':
#			MDN = "8201690102000"
#			URL = "http://www.magicn.com/"
#		elif sys.argv[1]=='url':
#			MDN = "820162010022"
#			URL = "http://www.magicn.com/oO???oc?UA????????UA???U®?????eø??Oⓒ???|º??|"
#		elif sys.argv[1]=='help' or sys.argv[1]=='?':
#			print "usage: port mdn url"
#			print "mdn = 820161002000, 8201690102000"
#			sys.exit()
#
#	if len(sys.argv) >= 4:
#		PORT = int(sys.argv[1])
#		MDN = sys.argv[2]
#		URL = sys.argv[3]

	#scheme, host, path, params, query, fragment =\
		#urlparse.urlparse(URL)

	#print "## Paras: ", HOST, URL, host, MDN
	#msg = REQMSG % (URL, host, MDN, MDN)	

###########################################################################
#	Brush main 
###########################################################################
	if len(sys.argv) != 4:
		print 'Usuage: python kunbrowser.py Host Mdn Depth'
		sys.exit(0)

	HOST = sys.argv[1]
	MDN = sys.argv[2]
	depth = int(sys.argv[3])

	#HOST = 'localhost'    # The remote host
	#MDN = "820160000000"    # 과금 NO

	print "------------------------------------------------------------------"
	print " TEST Started.. "
	print "------------------------------------------------------------------"

	filename = 'robot.log'				# robot.log 에 결과 기록 
	fp = open(filename, 'a')

	#client = KunClient(host=HOST, port=PORT) 		
	#client.conn() 						# conn Method return 1, if success

	client = KunClient()		 			# make instance
	client.header.parse(REQMSG)				# make initial Head 0.9
	client.header.add("HTTP_PHONE_NUMBER", MDN)
	client.header.add("HTTP_MDN_INFO", MDN)
	#client.header.add("Host", host)

	cj = cookielib.CookieJar()
	cookie_handler = urllib2.HTTPCookieProcessor(cj)
	#proxy_support = urllib2.ProxyHandler({'http':'http://localhost:8080'})
	proxy_support = urllib2.ProxyHandler({'http':'http://127.0.0.1:4444'})
	#http_handler = urllib2.HTTPHandler(debuglevel=1)	# DEBUG SHOW OPTION ON
	http_handler = MyHTTPHandler(debuglevel=1)	# DEBUG SHOW OPTION ON
	http_redirect = MyHTTPRedirectHandler(fp)

	handlers = [ http_handler, proxy_support, cookie_handler, http_redirect ]
	#handlers = [ http_handler, proxy_support, cookie_handler ]

	#opener = urllib2.build_opener(handler, proxy_support, urllib2.HTTPCookieProcessor(cj))
	opener = urllib2.build_opener(*handlers)
	urllib2.install_opener(opener)

	theurl = 'http://m.naver.com' 			# Naver require exact phone number
	theurl = 'http://m.dosirak.com'
	theurl = 'http://www.python.or.kr'
	theurl = 'http://m.ktf.com'
	theurl = 'http://m.yahoo.co.kr/'
	theurl = 'http://m.yahoo.com'
	theurl = 'http://www.magicn.com/'


	txdata = None					# txdata is BODY 
	txheaders = client.header.header_table		# txheaders is header
	#assert None

	#try:
	visited_list = []	# 방문한 곳 List
	url_table = {}	# URL 과 depth 가 저장되어 있는 Dictionary
	#depth = 2
	seq_num = 1
	prefix = 'prefixtest.com'

	url_table[theurl] = 1
	#print 'INIT URL_TABLE: '
	#print url_table

	while True:
		esc_flag = True
		for urls in url_table.keys():
			current_depth = url_table.get(urls)
			if current_depth <= depth:
				#print 'url_table.get valus : ', url_table.get(urls)
				#print 'depth is:  ', depth
				#print 'debug end -----------'
				esc_flag = False
				try:
					url_table.pop(urls) 	# Remove Visited URL

					if visited_list.count(urls) > 0:    # 방문한 곳이라면
						continue
					visited_list.append(urls)	#방문한곳에 현재사이트 추가

					scheme, host, path, params, query, fragment = \
						urlparse.urlparse(urls)

					if host.find(prefix) != -1:	# 필터링 되어야 하는곳은제외
						continue

					start = time.time()
					req = urllib2.Request(urls, txdata, txheaders)
					handle_output = urllib2.urlopen(req)
					end = time.time()

#						print "=" * 50, 'Begin'
#						print "req.headers"
#						print req.headers
#						print len(req.headers.__str__())
#						print dir(req.headers)
					length = 0
					for i in req.unredirected_hdrs:
						length = length + len(i) + len(req.unredirected_hdrs[i])
#						print req.unredirected_hdrs
#						print req.header_items()
#						print req.has_header('Cookie')
#						print "=" * 50, 'End'

					data = handle_output.read()

					if handle_output.headers.getheader('Content-Type') != \
						'text/html':	# Content-Type 이 아니라면 Skip
						continue

					#print data
					try:
						parser = hparser.hparser(handle_output.geturl(), debug=1)
						parser.feed(data)
						parser.close()
						lists = parser.get_anchors() # lists 에 body의 URL 저장
					except: 
						pass


					for homepage in lists:

						if urlparse.urlparse(homepage[0]) [0] != 'http':
							print urlparse.urlparse(homepage[0]) [0] 
							continue
						url_table[ homepage[0] ] = current_depth + 1

						#print 'DEBUG 4444'
						#print '-' * 55
						#print url_table
						#print '-' * 55
						# homepage[0] 는 URL homepage[1] 은 Value

					fp.write(time.asctime()[ 4 : ] + ' ')	# 시간기록
					fp.write(repr(seq_num) + ' ')		# Seq 기록
					seq_num = seq_num + 1
					#fp.write(repr(depth) + ' ')
					fp.write(repr(current_depth) + ' ')	# Depth 기록
					fp.write(repr(len(req.headers.__str__()) + length+6) + '/0 ')

					fp.write(repr(len(handle_output.info().__str__())) + '/')
					fp.write(repr(len(data)) + ' ')		# Data Size 기록
					fp.write(repr(handle_output.code) + ' ') # Status Code기록
					fp.write('%.3s' % (end - start) + ' ')	# Response Time기록
					fp.write(handle_output.geturl())	# URL 기록
					fp.write('\n')

				except IOError, e:
					if hasattr(e, 'read'):

						end = time.time()
						data = e.read()

						fp.write(time.asctime()[ 4 : ] + ' ')
						fp.write(repr(seq_num) + ' ')
						seq_num = seq_num + 1
						fp.write(repr(current_depth) + ' ')
						fp.write(repr(len(req.headers.__str__())) + '/0 ')
						fp.write(repr(len(e.info().__str__())) + '/')
						fp.write(repr(len(data)) + ' ')
						fp.write(repr(e.code) + ' ')
						fp.write('%.3s' % (end - start) + ' ')
						fp.write(e.geturl())
						fp.write('\n')
					if hasattr(e, 'geturl'):
						print 'We failed to open "%s"' % e.geturl()
					if hasattr(e, 'code'):
						print 'We failed with error code %s' % e.code
					if hasattr(e, 'reason'):
						print e.reason


		if esc_flag == True:
			break

	print 'LAST_URL_TABLE'
	print url_table
	print '-' * 50
	print visited_list
	fp.close()

#		print "=" * 50
#		print "req.headers"
#		print req.headers
#		print type(req.headers)
#		print req.unredirected_hdrs
#		print req.header_items()
#		print req.has_header('Cookie')
#		print req.has_header('Accept-encoding')
#		print "=" * 50
#
#		print 'Code Status: ',
#		print handle.code
#		print 'Host: ',
#		print handle.geturl()
#		print 'Respond body size: ',
#		print len(data)				#Respond body
#		#print handle.info()
#		print 'Respond header size: ',
#		print len(str(handle.info()))		#Respond header

	####################    LOG FILE  #######################

	#print '----------------------------------Second Request----------------------------------'

	#req = urllib2.Request(theurl, txdata, txheaders)
	#handle = urllib2.urlopen(req)

#	except IOError, e:
#		print 'We failed to open "%s"' % theurl
#		if hasattr(e, 'code'):
#			print 'We failed with error code %s' % e.code
#		elif hasattr(e, 'reason'):
#			print e.reason
#		sys.exit()
#	else:
#		pass
		#print "HEADER"
		#print handle.info()

	if cj is None:
		print "no cookie"
	else:
		print "cookies..."
		for index, cookie in enumerate(cj):
			print index, ' : ', cookie

#########

	#while 1:
		##CMD = raw_input(" Z:\<KunBrowser> ")
		#if CMD == "go":

			#if client.recheader.getStatusCode() == "":			# IF First LOOP (Execution)
				#message = client.header.build('http://magicn.com') 	# make initial Head 1.0
				#result = client.sendmsg(message)			# Send Header
				#client.recheader.parse(result)				# Parsing Received Head, Body

			#elif client.recheader.getStatusCode() == "302":			# IF HTTP 302 Response
				#scheme, host, path, params, query, fragment = \
					#urlparse.urlparse(client.recheader.getValue('Location'))
				#client.header.replace("Host", host)

				#message = client.header.build(client.recheader.getValue('Location'))	# GET by New Location

				#result = client.sendmsg(message)
				#client.recheader.parse(result)				# Parsing Received Head, Body

####### Some comment to remember 
####### header - will send - after once parse() - slow change value
######  recheader - received - after several parse() - ALL change
######  setcookie  module setting  :cookielib


			#print 'recheader test -----------------'
			#print client.recheader.header_table
			#print client.recheader.getBody()
			#print client.recheader.getStatusCode()


		#else: break
		
	#client.close() # socket close Method

########################### THIS IS TEST VERSION

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
	##assert hoho.header_table
	#print hoho.header_table
        #print hoho.getValue('Cookie')
        #print hoho.length()
        #hoho.replace('Host', 'brushzzang')
        #print hoho.build()
	
	##################################################################

