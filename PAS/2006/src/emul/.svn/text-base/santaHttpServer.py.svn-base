#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# SANTA HTTP Server Emulator

import BaseHTTPServer
import cgi
import sys
import re

# handol@gmail.com
# 2006.09.05


class aclDict:
	def __init__(self):
		self.D = {}
	
	def add(self, key, value):
		try:
			self.D[key] = value
		except:
			return
	
	def search(self, key):
		try:
			return self.D[key]
		except:
			return None
			
	# print  all data
	def prn(self):
		print "## len = %d" % len(self.D)
		for k,v in self.D.iteritems():
			print k,'\t',v
			
	# load data from the given data file
	def load(self, fname):
		try:
			fd = open(fname, 'r')
		except:
			print "cannot read", fname
			return
		
		for line in fd:
			line = line.strip()
			if len(line)==0 or line[0]=='#': continue
			flds = line.split()
			#print flds
			if len(flds) == 2:
				self.add(flds[0], flds[1])
			else:
				self.add(flds[0], flds[1:])

#  SANTA 요청 메시지 형식.
# http://SANTA_ADDR:SANTA_PORT/apps/sus?SVCID=svcid&ID=id&PASSWORD=password&FUNC=funcval
# http://URL?SVCID=testsvcd&ID=testid&PASSWORD=testpass&FUNC=GET(MSIN=0189990000,IMSI+MDN)
# FUNC 예시 : FUNC=GET(MSIN=0189990000,IMSI+MDN)

# SANTA 응답 메시지 형식
# RESPONSE=s&IMSI=4500001899990000&MDN=0115555555
class SantaReq:
	def __init__(self):
		self.D = {}
		
	def parse(self, req):
		pos = req.find('?')
		if pos == -1: 
			return -1
		
		flds = req[pos+1:].strip().split('&')
		
		#print flds
		for fld in flds:
			kv = fld.split('=')
			if len(kv) < 2: return -1
			
			try:
				if len(kv)==2:
					self.D[kv[0].upper()] = kv[1]
				else:
					self.D[kv[0].upper()] = '='.join(kv[1:])
			except:
				pass
		
		return 0

	# print  all data
	def prn(self):
		for k,v in self.D.iteritems():
			print k,'\t',v
	
	def getMSIN(self):
		try:
			func = self.D["FUNC"]
		except:
			return ""
		
		#print func
		m = re.match(".+MSIN=(\d+)", func)
		if m:
			return m.group(1)
		else: 
			return ""
			
	def authorize(self, aclDict):
		try:
			if self.D["SVCID"] != aclDict.search("SVCID"):
				return -1, "response=err_svcid"
		except:
			return -1, "response=err_svcid"
			
		try:
			if self.D["ID"] != aclDict.search("ID"):
				return -1, "response=err_id"
		except:
			return -1, "response=err_id"
		
		try:
			if self.D["PASSWORD"] != aclDict.search("PASSWORD"):
				return -1, "response=err_password"
		except:
			return -1, "response=err_password"
			
		return 0, ""
	
	
	
	def handle(self, aclDict, reqPara):
		# parse reqPara
		if self.parse(reqPara) < 0:
			return -1, "response=err_param"
		
		res, msg = self.authorize(aclDict)
		if res < 0:
			return res, msg
		
		msin = self.getMSIN()
		print "MSIN=", msin
		
		val = aclDict.search(msin)
		
		if val==None or len(val)==1:
			return -1, "response=err_noreply"
		else:
			msg = "RESPONSE=s&IMSI=%s&MDN=%s" % (val[0], val[1])
			return 0, msg
		
		

class Handler(BaseHTTPServer.BaseHTTPRequestHandler):
	#def __init__(self):
	#	BaseHTTPServer.BaseHTTPRequestHandler.__init__(self)
	
	def load(self):
		global ACLDICT
		self.acl = ACLDICT

	def makeBody(self, mesg):
		# generate a random message
		#print "<html>"
		#print "<body>"
		#return cgi.escape(mesg)
		return mesg
		#print "</body>"
		#print "</html>"
		
	def do_GET(self):
		print "FROM client: ", self.path
		self.load()
		santaReq = SantaReq()
		resCode, resMesg = santaReq.handle(self.acl, self.path)
		
		#if resCode != 200:
		#	self.send_error(resCode, resMesg)
		#	return
		
		resMesg = resMesg.strip()
		print "resMesg:", resMesg
		bodymsg = self.makeBody(resMesg)
		print "TO client: ", bodymsg 

		self.send_response(200)
		self.send_header("Content-type", "text/html;charset=euc-kr")
		self.send_header("Content-Length", "%d" % len(bodymsg))
		self.end_headers()
		try:
			# redirect stdout to client
			stdout = sys.stdout
			sys.stdout = self.wfile
			self.wfile.write(bodymsg)
		finally:
			sys.stdout = stdout # restore
		
	

def test(aclfile):
	print "### ACL data"
	acl = aclDict()
	acl.load(aclfile)
	acl.prn()
	
	paths = [
		"/apps/sus?SVCID=hello&ID=pas2&PASSWORD=pas2&FUNC=GET(MSIN=0182304250,MDN+IMSI)",
		"/apps/sus?SVCID=mapexam&ID=pas2&PASSWORD=pas2&FUNC=GET(MSIN=0182304250,MDN+IMSI)",
		"/apps/sus?SVCID=mapexam&ID=pas&PASSWORD=pas2&FUNC=GET(MSIN=0182304250,MDN+IMSI)",
		"/apps/sus?SVCID=mapexam&ID=pas&PASSWORD=pas&FUNC=GET(MSIN=0182304250,MDN+IMSI)",
		"/apps/sus?SVCID=mapexam&ID=pas&PASSWORD=pas&FUNC=GET(MSIN=01070009200,MDN+IMSI)",
		""
	]

	for p in paths:
		if p=="": break
		print "# test:", p
		santaReq = SantaReq()
		santaReq.prn()
		resCode, resMesg = santaReq.handle(acl, p)
		print resCode, resMesg

	

if __name__ == "__main__":
	global ACLDICT

	PORT = 8000
	
	if len(sys.argv) > 2 and sys.argv[1]=="test":
		test(sys.argv[2])
		sys.exit()

	if len(sys.argv)==3:
		PORT = int(sys.argv[1])
	else:
		print "usage: santa_port acl_data_file"
		sys.exit(0)
	
	ACLDICT = aclDict()
	ACLDICT.load(sys.argv[2])
		
	httpd = BaseHTTPServer.HTTPServer(("", PORT), Handler)
	
	print "serving at port", PORT
	httpd.serve_forever()
