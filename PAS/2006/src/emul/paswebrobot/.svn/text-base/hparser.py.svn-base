#!/usr/bin/env python
import HTMLParser
import urlparse

UNPAIRED_TAGS = [ 'meta', 'img', 'br', 'hr', 'col', 'input', 'index', 
					'basefont', 'link', 'base', 'area', 'param', 'isindex'  ]

NODATA_TAGS = [ 'script', 'style' ]

def swap_last_two(list):
	a = list.pop()
	b = list.pop()
	list.append(a)
	list.append(b)

class hparser(HTMLParser.HTMLParser):
	def __init__(self, baseurl, debug=0):					
		HTMLParser.HTMLParser.__init__(self)
		self.baseurl = baseurl
		self.data = ''
		self.debug = debug
		self.anchors = []
		#self.names = []
		self.is_anchor = False
			
	def close(self):
		pass

	def filter_anchor_url(self,a_url):
		if a_url[0]=='#': return 0
		if a_url.find(':') >= 0:
			if a_url.startswith('mailto')\
				or a_url.startswith('java'):
				return 0
		return 1

	def get_href_url(self, attrs):
		for name,value in attrs:
			if name=='href':
				if self.filter_anchor_url(value):
					return value
		return ''
	
	def do_anchors(self, attrs):
		#print attrs
		try:
			tmpurl = self.get_href_url(attrs)
		except:
			return

		if tmpurl != '':
			newurl = urlparse.urljoin(self.baseurl, tmpurl)
			#print tmpurl,'-->',newurl
			if newurl not in self.anchors:
				self.anchors.append( [newurl, None] )

	def handle_starttag(self, tag, attrs):
		if tag == 'a':
			self.do_anchors(attrs)
			self.is_anchor = True
		else:
			self.is_anchor = False

	def handle_startendtag(self, tag, attrs):
		self.is_anchor = False
		
	def handle_endtag(self, tag):		
		self.is_anchor = False

	def handle_data(self, data):
		data = data.strip()
		print data
		if self.is_anchor is True and data:
			self.anchors[-1][1] = data
			#print self.is_anchor
			#print 'data is:',data,'end'
			#print '*' * 55
			#print self.anchors
			#print '*' * 55
		self.is_anchor = False

	def report_unbalanced(self, tag):
		print '???', tag
		self.depth -= 1

#######################################################################################
#	DELETE
#######################################################################################
		
	def prn_anchors(self):
		for url,ref in self.anchors:
			print "%s -- [%s]" % (url, ref)

	def get_anchors(self):
		return self.anchors

##########################



class myagent:
        def __init__(self, bodydata, debug=0):
                """
                get HTML contents at a given url 'urlstr'
                """
                self.debug = debug
                #self.geturl = geturl.geturl(urlstr)

                #if debug:
                        #print '### DATA size:', len(self.geturl.data)

                self.parser = hparser('http://kunhost.com', debug=1)
                self.parser.feed( bodydata )
                self.parser.close()
                #if debug:
                        #print '### Got :', len(self.parser.data)
                        #print self.parser.data

                print '#'*50,'\n'
                print self.parser.prn_anchors()

if __name__ == "__main__":
        import sys
        url = "http://www.python.org"
        url = "http://www.google.co.kr"

        save = ''
        if len(sys.argv) > 1: url = sys.argv[1]
        if len(sys.argv) > 2: save = sys.argv[2]

	f = open('input.html', 'r')
	bodydata = f.read()
        a = myagent(bodydata, debug=0)


