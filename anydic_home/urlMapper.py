from urlparse import urlparse

DEF_AD_HOME = "/home/anydict/"
DEF_MAPPER_FILE = "mapper.dat"
class urlMapper:
	def __init__(self):
		pass
	
	def load(self):
		try:
			fd = open(DEF_AD_HOME+DEF_MAPPER_FILE, 'r')
		except (OSError, IOError), e:
			print str(e)	
			return None

		L=[]
		for l in fd.readlines():
			cols = l.split()
			#print cols
			if cols==[] or cols[0][0]=='#':
				continue
			L.append(cols)
		return L
		pass
	
	def save(self):
		pass
			
	def getHost(self, url):
		print "url= ", url
		if url.startswith("http://"):
			print "YES"
			s = urlparse(url)
		else:
			print "NO"
			s = urlparse(url, "http")
		print s	
		print "host part: ", s[1]
		port_pos =  s[1].rfind(':')
		if port_pos >= 0:
			s[1] = s[1][:port_pos]
		print "host part: ", s[1]
		
		h = s[1].split('.')
		d_len = len(h[-1]) # www.cnn.com; d_len=len('com')
		if d_len>=3 and len(h)>=2:
			return h[-2]
		elif d_len<3 and len(h)>=3:
			return h[-3]
		else:
			return ""
				
	def insert(self, L):
		"insert into dict"
		D={}
		for l in L:
			h = self.getHost(l[0])
			print l[0], "-->", h
			if h!="":
				D[h] = l[0]
		return D	
		pass
		
	def search(self):
		pass
	
	def match(self):
		pass
		
	
	def __init__(self):
		pass

if __name__=="__main__":
	mapper = urlMapper()
	L = mapper.load()
	print L
	D = mapper.insert(L)
	print D.keys()
	print D.items()
