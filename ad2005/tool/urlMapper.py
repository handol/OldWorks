#!/usr/bin/env python

from urlparse import urlparse

DEF_AD_HOME = "/home/anydict/"
DEF_AD_HOME = "./"
DEF_MAPPER_FILE = "mapper.dat"
class urlMapper:
	L=[]
	D={}
	def __init__(self):
		pass
	
	def load(self):
		try:
			fd = open(DEF_AD_HOME+DEF_MAPPER_FILE, 'r')
		except (OSError, IOError), e:
			print str(e)	
			return None

		for l in fd.readlines():
			cols = l.split()
			#print cols
			if cols==[] or cols[0][0]=='#':
				continue
			self.L.append(cols)
		return
	
	def save(self):
		pass
			
	def getHost(self, url):
		#print "url= ", url
		if url.startswith("http://"):
			s = urlparse(url)
		else:
			s = urlparse("http://"+url)
		#print s	

		# trail out port_number part
		port_pos =  s[1].rfind(':')
		if port_pos >= 0:
			hp = s[1][:port_pos]
		else:
			hp = s[1]
		
		h = hp.split('.')
		d_len = len(h[-1]) # www.cnn.com; d_len=len('com')
		if d_len>=3 and len(h)>=2:
			return h[-2]
		elif d_len<3 and len(h)>=3:
			return h[-3]
		else:
			return "---"
				
	def insert(self, debug=False):
		"insert into dict"
		n=0
		for l in self.L:
			h = self.getHost(l[0])
			if debug: print l[0], "-->", h
			if h!="---":
				n += 1
				if h in self.D:
					self.D[h].append(l[0])
				else:
					self.D[h] = [l[0]]

		print "%d URLs inserted" % n
		return
		
	def prnList(selft, list):
		for l in list:
			print "\t", l

	def search(self, keyhost):
		try:
			self.prnList(self.D[keyhost])	
		except:
			print "No in the list"
		pass
	
	def match(self):
		while 1:
			try:
				a = raw_input("Enter a host name:").strip()
				if a=='quit': break
				if a=='' or '?':
					self.prnList(self.D.keys())
				else:
					mapper.search(a)
			except: 
				print
				break
		
	
	def __init__(self):
		pass

if __name__=="__main__":
	mapper = urlMapper()
	mapper.load()
	#print mapper.L
	mapper.insert()
	#print mapper.D.keys()
	#print mapper.D.items()
	mapper.match()

