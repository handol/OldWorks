#!/usr/bin/env python
#Copyright Jon Berg , turtlemeat.com
# -*- coding: EUC-KR -*-

import string,cgi,time
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
#import pri

class MyHandler(BaseHTTPRequestHandler):

	def do_GET(self):
		try:
			today = time.strftime("%m%d")
			outf = open("webreq.%s.log" % today, "a+")
			now = time.strftime("%m/%d %H:%M:%S")
			outf.write("%s recv request\n" % now)
			outf.close()
		except:
			pass

		try:

			f = open("index.html")
			body = f.read()
			f.close()

		except IOError:
			body = """<html><body> \
			<a href="javascript:history.back()">다시 한번 시도해 주세요</a> \
			</body></html> \
			"""
			#self.send_error(404,'File Not Found: %s' % self.path)

		self.send_response(200)
		self.send_header('Content-type','text/html')
		self.send_header('Content-length', len(body) )
		self.end_headers()
		self.wfile.write(body)
			
		
	 

	def do_POST(self):
		global rootnode
		try:
			ctype, pdict = cgi.parse_header(self.headers.getheader('content-type'))
			if ctype == 'multipart/form-data':
				query=cgi.parse_multipart(self.rfile, pdict)
			self.send_response(301)
			
			self.end_headers()
			upfilecontent = query.get('upfile')
			print "filecontent", upfilecontent[0]
			self.wfile.write("<HTML>POST OK.<BR><BR>");
			self.wfile.write(upfilecontent[0]);
			
		except :
			pass

def main(portnum):
	try:
		server = HTTPServer(('', portnum), MyHandler)
		print 'started httpserver...'
		print 'port = %d' % (portnum)
		server.serve_forever()
	except KeyboardInterrupt:
		print '^C received, shutting down server'
		server.socket.close()

import sys

if __name__ == '__main__':
	if len(sys.argv) == 2:
		port = int(sys.argv[1])
	else:
		print "Usage: port_number"
		sys.exit(0)

	main(port)

