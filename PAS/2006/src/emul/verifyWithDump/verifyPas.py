#!/usr/bin/env python
#-*- coding: EUC-KR -*-

import time
import sys

import TcpServer
import TcpClient

def countSec(hhmmss):
	flds = hhmmss.split(":")
	if len(flds) < 3: 
		return -1
		
	vals = map(int, flds)
	sec = vals[0] * 60 * 60 + vals[1] * 60 + vals[2]
	return sec
	
def currday():
	#return time.strftime('%Y%m%d')
	tm = time.localtime()
	return "%d%02d%02d" % (tm.tm_year, tm.tm_mon, tm.tm_mday)
	
def currhour():
	#return time.strftime('%Y%m%d%H')
	tm = time.localtime()
	return "%d%02d%02d%02d" % (tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour)


def getUrl(line):
	flds = line.split()
	if len(flds) != 3:
		return ''
	
	if not flds[2].startswith("HTTP"):
		return ''
	
	return flds[1]

def getPhoneNum(line):
	flds = line.split(':')
	if len(flds) < 2:
		return ''
		
	pnum = flds[1].strip()
	if pnum.startswith("82"):
		pnum = pnum[2:]
	if not pnum.startswith("0"):
		pnum = "0" + pnum	
	return pnum

def parsePhoneNum(lines):
	for line in lines:
		if line.find("PHONE_NUMBER") != -1:
			return getPhoneNum(line)
	return ''

def getContentLength(lines):
	for line in lines:
		if line.lower().find("content-length") != -1:
			flds = line.split(":")
			if len(flds) < 2:
				return 0				
			return int(flds[1])
	return 0

# 헤더에 chunked 가 있는 경우 content-length 형태로 변환하여 응답
def replaceChunked(lines, leng):
	idx = 0
	for line in lines:
		if line.lower().find("transfer-encoding") != -1:
			newline = "Content-Length: %d\r\n" % (leng)
			lines[idx] = newline
			return lines
		idx += 1
	return []

class PasLogReader:
	def __init__(self):
		self.index = 0
		self.linecnt = 0
		pass
		
	def open(self, fname=""):
		if fname=="":
			self.fd = None
			return
			
		try:
			self.fd = open(fname, "r")
		except:
			print "reading fail", fname
			return
	
	def find(self, phonenum, url, hhmmss):
		if self.fd == None:
			return
		
		dumpSec = countSec(hhmmss)
		
		for line in self.fd:
			self.linecnt += 1
			flds = line.split()
			if len(flds) < 13:
				print "wrong number of fields in line %d" % (self.linecnt)
				continue
			
			if flds[2]==phonenum and flds[12]==url:
				logSec = countSec(flds[1])
				diffSec = abs(logSec - dumpSec)
				print 
				print "Search: %s %s" % (phonenum, url)
				print "Found Line: %d" % (self.linecnt)
				print "Diff sec: %d" % (diffSec)
				print line
				print
				break
			

def getHeaderStr(lines, idx):
	if len(lines) < 2:
		return '', ''

	if idx >= len(lines):
		return '', ''

	for i in range(idx, len(lines)):
		if lines[i].startswith("##"):
			break
		if len(lines[i]) <= 2 and lines[i][-1] == '\n':
			break
	
	header = ''.join(lines[idx:i+1])
	if i >= len(lines)-1:
		body = ''
	else:
		body = ''.join(lines[i+1:])
	return header, body
	
	
class DumpProc:
	def __init__(self, client, server, debug=0):
		self.phone = client
		self.cpweb = server
		self.debug = debug
		self.isError = False
		self.foundPH = False
		self.foundCP = False
	
	def setCpResp(self):
		pass
		
	def sendPhoneReq(self):
		pass
	
	def see(self, fname, logfile=""):
		self.phReqCnt = 0

		self.fname = fname
		try:
			fd = open(fname, "r")
		except:
			print "reading fail", fname
			return
		
		self.pasLog =  PasLogReader()
		self.pasLog.open(logfile)		
			
		sharpLineCnt = 0
		nonSharpLineCnt = 0
		lines = []
		for line in fd:
			if line=="##\n":
				try:
					lines.pop()
				except:
					pass
					
				self.feed(lines)
				lines = []
			else:
				lines.append(line)
		
		self.feed([])		
		
	def feed(self, lines):
		if lines == []:
			self.procOneTr()
			return
			
		idx = 0
		while idx < len(lines) and not lines[idx].startswith("###"):
			idx += 1
		
		if idx != 0:
			lines = lines[idx:]
		
		if len(lines) < 3:
			print "missing lines"
			return
		
		
		if lines[0][4]=='P':
			self.foundPH = True
			if (self.phReqCnt > 0):
				self.procOneTr()
						
			self.phReqCnt += 1					
			print "%s == PHONE REQ %d" % (self.fname, self.phReqCnt)			
			self.parsePhoneReq(lines)
			if self.debug: self.prnPhone()
			
		else:
			self.foundCP = True
			print "%s == CP RESP %d" % (self.fname, self.phReqCnt)
			self.parseCpResp(lines)
			if self.debug: self.prnCp()
			
	def procOneTr(self):
		# self.pasLog.find(self.phonenum, self.url, self.ph_hhmmss)
		print "One Tr: %d" % (self.phReqCnt)

		if not self.foundPH or  not self.foundCP:
			print "Wrong(CP, PH) in dump data"
			self.foundPH = False
			self.foundCP = False
			return

		if self.isError:
			print "Wrong in dump data"
			self.isError = False
			return

		self.foundPH = False
		self.foundCP = False

		# some error or exception in DUMP data
		if self.ph_size > 1300:
			print "Phone Req Mesg too long: skip test"
			return

		# some error or exception in DUMP data
		phHeader = self.phoneHeader.upper()
		if not phHeader.startswith("GET") and not phHeader.startswith("POST"):
			print "Phone Req Mesg is wrong : skip test"
			return
			

		TcpServer.setRespMesg(self.cpHeader, self.contentLeng)
		if self.phContentLeng > 0:
			print "Phone REQ: H=%d B=%d" % (len(self.phoneHeader), self.phContentLeng)
			self.phone.send(self.phoneHeader + self.phContentLeng*'p')
		else:
			self.phone.send(self.phoneHeader)
			
		#time.sleep(0.2)
		timea = time.time()
		while self.phone.recvDataSize < self.contentLeng:
			time.sleep(0.1)
			timeb = time.time()
			if timeb - timea > 3:
				print "ERROR: timeout - no result from PAS"
				break
		time.sleep(0.1)
		
	def parseCpResp(self, lines):
		if len(lines) < 5:
			self.isError = True
			return
		self.hhmmss = lines[0].split()[2]	
		flds = lines[1].split()
		if len(flds) >= 3:
			self.seq = int(flds[1])
			self.cpSeq = int(flds[2])
		else:
			self.seq = -1
			self.cpSeq = -1
		
		sizes = lines[2].split()[1]
		self.cp_size = int(sizes.split('/')[1])
		
		if self.cpSeq == 1:
			self.contentLeng = getContentLength(lines)
			if self.contentLeng == 0:
				print "NO Content-Leng"
				newlines = replaceChunked(lines, self.cp_size)
				if len(newlines) > 0:
					print "Chunked"
					lines = newlines
					self.contentLeng = self.cp_size
				
			if lines[3][0]=='#':
				self.cpHeader, self.cpBody = getHeaderStr(lines, 4)
				
				statusline = lines[4]
			else:
				self.cpHeader, self.cpBody = getHeaderStr(lines, 3)				
				statusline = lines[3]
				
			self.statusLine = statusline
			self.code = int (statusline.split()[1])
						
			
	def prnCp(self):
		print "time: %s" % (self.hhmmss)
		print "seq: %d %d" % (self.seq, self.cpSeq)
		print "size: %d" % (self.cp_size)
		print "Status: %s" % (self.statusLine)
		print "Code: %s" % (self.code)
		print "contentLeng: %d" % (self.contentLeng)
			
	def parsePhoneReq(self, lines):
		if len(lines) < 5:
			self.isError = True
			return
			
		self.ph_hhmmss = lines[0].split()[2]

		flds = lines[1].split()
		if len(flds) == 3:
			self.seq = int(flds[1])
			self.phoneSeq = int(flds[2])
		else:
			self.seq = -1
			self.phoneSeq = -1
			
		self.sizes = lines[2].split()[1]
		self.ph_size = int(self.sizes.split('/')[1])
		
		if self.phoneSeq == 1:
			if lines[3][0]=='#':				
				self.phoneHeader, self.phoneBody = getHeaderStr(lines, 4)
				self.url = getUrl(lines[4])
				self.phonenum = parsePhoneNum(lines)
			else:
				self.phoneHeader, self.phoneBody = getHeaderStr(lines, 3)
				self.url = getUrl(lines[3])
			
			self.phContentLeng = getContentLength(lines)
			if self.url=='':
				pass
				
	def prnPhone(self):
		print "time: %s" % (self.ph_hhmmss)
		print "seq: %d %d" % (self.seq, self.phoneSeq)
		print "size: %d" % (self.ph_size)
		print "url: %s" % (self.url)
		print "phone: %s" % (self.phonenum)
		
	
import os, fnmatch
def all_files(root, patterns='0*', single_level=False, yield_folders=True):
    # Expand patterns from semicolon-separated string to list
    patterns = patterns.split(';')
    for path, subdirs, files in os.walk(root):
        if yield_folders:
            files.extend(subdirs)
        files.sort( )
	print files
        for name in files:
            for pattern in patterns:
                if fnmatch.fnmatch(name, pattern):
                    fullname = os.path.join(path, name)
		    if os.path.isfile(fullname):
		    	yield fullname
                    break
        if single_level:
            break

from stat import *

# return the list of files
def walktree(dir, prn=0):
	outlist = []
	for f in os.listdir(dir):
		pathname = os.path.join(dir, f)
		if prn: print pathname
		
		try:
			stinfo = os.stat(pathname)
		except OSError:
			continue

		mode = stinfo[ST_MODE]
		if S_ISDIR(mode):
			# It's a directory, recurse into it
			res = walktree(pathname, prn)
			outlist += res
		elif S_ISREG(mode):
			# 숫자로 이루어진 전화 번호만.
			if f[0].isdigit() and f[-1].isdigit():
				outlist.append(pathname)
	outlist.sort()
	return outlist
		
if __name__ == "__main__":
	if len(sys.argv) < 5:
		print "Usage: dump_file_or_dir pas_addr pas_port cpwebserver_port [begin] [how_many]"
		sys.exit()

	if sys.argv[1][-1] != "/":
		cli = TcpClient.TcpClient()
		cli.conn(sys.argv[2], int(sys.argv[3]))
		cli.start()
		
		cpsvr = TcpServer.MyServer(int(sys.argv[4]))
		cpsvr.start()
		
		proc = DumpProc(cli, cpsvr)
		
		proc.see(sys.argv[1])
		time.sleep(1)
		cli.stop()
		cpsvr.stop()
	
	else:
		
		cpsvr = TcpServer.MyServer(int(sys.argv[4]))
		cpsvr.start()
		
		
		cnt = 0
		
		#for dumpfile in all_files(sys.argv[1]):
		all_mdns = walktree(sys.argv[1])
		print "All %d Files" % (len(all_mdns))

		if len(sys.argv) > 5:
			begin = int (sys.argv[5])
			if len(sys.argv) > 6:
				many = int (sys.argv[6])
				end = begin + many
				if end > len(all_mdns):
					end = len(all_mdns)
			else:
				end = len(all_mdns)


			print "BEGIN=%d END=%d" % (begin, end)

			all_mdns = all_mdns[begin:end]

		for dumpfile in all_mdns:
			cnt += 1
			print "== [%4d] %s" % (cnt , dumpfile)

			cli = TcpClient.TcpClient()
			cli.conn(sys.argv[2], int(sys.argv[3]))
			cli.start()

			proc = DumpProc(cli, cpsvr)


			proc.see(dumpfile)
			cli.stop()

		time.sleep(1)
		cpsvr.stop()
		print "All %d Files" % (len(all_mdns))
	
