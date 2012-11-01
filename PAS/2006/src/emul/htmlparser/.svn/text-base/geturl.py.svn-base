#!/usr/bin/env python
import urllib
import urlparse

class geturl:
	def __init__(self, urlstr, savefile='', debug=0):
		"""
		get HTML contents at a given url 'urlstr'
		read html data into file or memory.
		"""			
		
		urltuple = urlparse.urlparse(urlstr, 'http')
		urlstr = urlparse.urlunparse(urltuple)
		print urlstr
		self.baseurl = urlstr
		self.fp = urllib.urlopen(urlstr)
		self.debug = debug
		self.savefile = savefile

		self.data = ''		
		if savefile != '':	save = open(savefile, "wb")
		else: save = None
		
		while 1:
			html = self.fp.read(1024*8)
			if not html: break

			if debug: print html
			if save: save.write(html)
			self.data += html
		self.fp.close()
		if save: save.close()		

	def head(self):
		print self.fp.headers		
		
	def res(self):
		print "read", len(self.data) , "bytes from", self.fp.url

if __name__ == "__main__":
	import sys
	url = "http://www.python.org"
	save = ''	
	if len(sys.argv) > 1: url = sys.argv[1]		
	if len(sys.argv) > 2: save = sys.argv[2]

	g = geturl(url, save)			
	g.head()
	g.res()
