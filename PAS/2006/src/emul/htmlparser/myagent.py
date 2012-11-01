#!/usr/bin/env python
import geturl
import hparser

class myagent:
	def __init__(self, urlstr, debug=0):
		"""
		get HTML contents at a given url 'urlstr'
		"""			
		self.debug = debug
		self.geturl = geturl.geturl(urlstr)
		if debug: 
			print '### DATA size:', len(self.geturl.data)

		self.parser = hparser.hparser(self.geturl.baseurl, debug=1)
		self.parser.feed( self.geturl.data )
		self.parser.close()
		if debug: 
			print '### Got :', len(self.parser.data)
			print self.parser.data
		self.parser.analyze()	
		print '#'*50,'\n'
		self.parser.analyze2()	
		print '#'*50,'\n'
		print self.parser.prn_anchors()

if __name__ == "__main__":
	import sys
	url = "http://www.python.org"
	#
	url = "http://www.google.co.kr"

	save = ''	
	if len(sys.argv) > 1: url = sys.argv[1]		
	if len(sys.argv) > 2: save = sys.argv[2]

	a = myagent(url, debug=1)			
