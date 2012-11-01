import time
import sys

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
			

			
class DumpProc:
	def __init__(self):
		self.cnt = 0
		
	def see(self, fname, logfile=""):
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
				self.feed(lines)
				lines = []
			else:
				lines.append(line)
		
		self.feed([])
		
	def feed(self, lines):
		idx = 0
		while idx < len(lines) and not lines[idx].startswith("###"):
			idx += 1
		
		if idx != 0:
			lines = lines[idx:]
		
		if len(lines) < 4:
			print "missing lines"
			return
			
		self.cnt += 1		
		
		findExec = True
		if lines[0][4]=='P':
						
			print "===== PHONE REQ %d" % (self.cnt)
			self.parsePhoneReq(lines)
			self.prnPhone()
			findExec = False
		else:
			print "===== CP RESP %d" % (self.cnt)
			self.parseCpResp(lines)
			self.prnCp()
			if self.cpSeq == 1:
				self.pasLog.find(self.phonenum, self.url, self.ph_hhmmss)
			findExec = True
		
		print
	
	def parseCpResp(self, lines):
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
			if lines[3][0]=='#':
				self.cpHeader = lines[4:]								
			else:
				self.cpHeader = lines[3:]
			self.statusLine = self.cpHeader[0].strip()
			self.code = int (self.statusLine.split()[1])
			self.contentLeng = getContentLength(self.cpHeader)			
	
	def prnCp(self):
		print "time: %s" % (self.hhmmss)
		print "seq: %d %d" % (self.seq, self.cpSeq)
		print "size: %d" % (self.cp_size)
		print "Status: %s" % (self.statusLine)
		print "Code: %s" % (self.code)
		print "contentLeng: %d" % (self.contentLeng)
			
	def parsePhoneReq(self, lines):				
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
				self.phoneHeader = lines[4:]
				self.url = getUrl(lines[4])
				self.phonenum = parsePhoneNum(lines)
			else:
				self.phoneHeader = lines[3:]
				self.url = getUrl(lines[3])
			
			if self.url=='':
				pass
				
	def prnPhone(self):
		print "time: %s" % (self.ph_hhmmss)
		print "seq: %d %d" % (self.seq, self.phoneSeq)
		print "size: %d" % (self.ph_size)
		print "url: %s" % (self.url)
		print "phone: %s" % (self.phonenum)
		
	

		
if __name__ == "__main__":
	if len(sys.argv) < 2:
		sys.exit()
	
	proc = DumpProc()
	
	if len(sys.argv) >= 3:
		proc.see(sys.argv[1], sys.argv[2])
	else:
		proc.see(sys.argv[1])