#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# SANTA HTTP Server Emulator

import BaseHTTPServer
import cgi
import sys
import re
import time

def getNumber(str):
	m = re.match("\D+(\d+)\D+", str)
	if m:
		#print m.group(1)
		return int(m.group(1))
	else: 
		return 0


class Handler(BaseHTTPServer.BaseHTTPRequestHandler):
	#def __init__(self):
	#	BaseHTTPServer.BaseHTTPRequestHandler.__init__(self)
	
	def makeBody(self, mesg):
		# generate a random message
		#print "<html>"
		#print "<body>"
		#return cgi.escape(mesg)
		return mesg
		#print "</body>"
		#print "</html>"
	
	def do_timeout(self):
		sec = getNumber(self.path)
		for i in range(sec):
			time.sleep(1)
		self.send_good("I slept %d secs !!" % sec)
	
	def do_error(self):
		code = getNumber(self.path)		
		self.send_errmsg("I sent %d codes. " % code, code)
	
	def send_errmsg(self, bodymsg, errcode):
		print "TO client: ", bodymsg 
		self.send_error(errcode)
		self.send_header("Content-type", "text/html;charset=euc-kr")
		self.send_header("Content-Length", "%d" % len(bodymsg))
		self.end_headers()		
		
		try:
			# redirect stdout to client
			stdout = sys.stdout
			sys.stdout = self.wfile
			print bodymsg
		finally:
			sys.stdout = stdout # restore
		
	def send_good(self, bodymsg):
		print "TO client: ", bodymsg 
		self.send_response(200)
		self.send_header("Content-type", "text/html;charset=euc-kr")
		self.send_header("Content-Length", "%d" % len(bodymsg))
		self.end_headers()
		
		try:
			# redirect stdout to client
			stdout = sys.stdout
			sys.stdout = self.wfile
			print bodymsg
		finally:
			sys.stdout = stdout # restore
		
	def do_GET(self):
		print "FROM client: ", self.path
		if self.path.startswith("/timeout"):
			self.do_timeout()
		elif self.path.startswith("/error"):
			self.do_error()
	
		
	

if __name__ == "__main__":

	PORT = 8000

	if len(sys.argv)==2:
		PORT = int(sys.argv[1])
	else:
		print "usage: port"
		sys.exit(0)
		
	httpd = BaseHTTPServer.HTTPServer(("", PORT), Handler)
	
	#print getNumber("fdaf10fds")
	#print getNumber("200")
	
	print "serving at port", PORT
	print "request me:  http://localhost/timeout_10.html"
	print "request me:  http://localhost/error_404.html"
	httpd.serve_forever()
