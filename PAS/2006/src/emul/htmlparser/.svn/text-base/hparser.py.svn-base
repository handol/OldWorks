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
		self.depth = 0
		self.stack = []
		self.data = ''
		self.debug = debug
		self.good = []
		self.anchors = []
		self._anchor_flag = 0
			
	def close(self):
		if self.debug:
			print '### Closing parser: line %d, col %d' % self.getpos()
			if len(self.rawdata):
				print	self.rawdata[:150]
				self.goahead(0)

	def	__pop(self, tag):
		if len(self.stack)==0:
			print '!! tag pair mismatch [%s] : ' % tag, 'line %d, column %d' % self.getpos()
			return []

		if tag not in self.stack:
			print '!! tag pair unmatched tag [%s] : ' % tag, \
				'line %d, column %d' % self.getpos()
			return []

		#if self.stack[-1] != tag and len(self.stack) > 1 and self.stack[-2] == tag:
		#	print '!!! tag mis-ordered : [%s] [%s]' % \
		#			(self.stack[-1], self.stack[-2]), \
		#		'line %d, column %d' % self.getpos()
		#	#swap_last_two(self.stack)

		poped = []
		while len(self.stack):
			poped.append( self.stack.pop() )
			if poped[-1]==tag:
				break

		self.depth = len(self.stack)
		#poped.reverse()
		return poped

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
	
	def	do_anchors(self, attrs):
		print attrs
		tmpurl = self.get_href_url(attrs)
		if tmpurl != '':
			newurl = urlparse.urljoin(self.baseurl, tmpurl)
			print tmpurl,'-->',newurl
			if newurl not in self.anchors:
				self.anchors.append( [newurl, None] )
				self._anchor_flag = 1

	def handle_starttag(self, tag, attrs):
		if tag not in UNPAIRED_TAGS:
			ment = 'S '
			if self.debug: print ' '*self.depth +'%-3d'%self.depth + ment, tag
			self.depth += 1
			self.stack.append(tag)
			if tag == 'a':
				self.do_anchors(attrs)
		else:
			ment = 'NP'
			if self.debug: print ' '*self.depth +'%-3d'%self.depth + ment, tag

	def handle_startendtag(self, tag, attrs):
		if self.debug:
			print ' '*self.depth +'%-3d'%self.depth + 'S/E '+ tag	
		
	def handle_endtag(self, tag):		
		poped = self.__pop(tag)
		if self.debug:
			if len(poped) > 1:
				msg = '*'*len(poped) +' ' + ','.join(poped)
				msg += ' line %d, column %d' % self.getpos()
			elif len(poped): 
				msg = poped[0]
			else:
				msg=''
			print ' '*self.depth +'%-3d'%self.depth + 'E '+ msg

	def handle_data(self, data):
		data = data.strip()
		if data != '':
			if self.stack[-1] not in NODATA_TAGS:
				if self.stack[-1] == 'a':
					#self.data = self.data.replace('\n',' ')
					if self._anchor_flag:
						self.anchors[-1][1] = data
						self._anchor_flag = 0
					pass
				self.data += data+'\n'
				# insert (tag, depth, contents)
				self.good.append( (self.stack[-1], len(self.stack), data ) )
			print ' '*self.depth, data

	def report_unbalanced(self, tag):
		print '???', tag
		self.depth -= 1

#######################################################################################
#	DELETE
#######################################################################################
		
	def __find_root(self, curdepth, curidx):
	# return index. return -1 if not found
		for i in xrange(curidx-1, -1, -1):
			if self.good[i][1] < curdepth:
				return i
		return -1

	def analyze(self):
		n = 0
		roots = []
		for info in self.good:
			print '%10s %2d %3d' % ( info[0], info[1], len(info[2]) ), info[2][:20]

	def find_group(self, startpos, prn=0):
		info = self.good[startpos]
		base_depth = info[1]
		sum = len(info[2])
		count = 1 
		data = ''

		n = startpos + 1
		while n < len(self.good):
			gap = base_depth - self.good[n][1]
			if abs(gap) > 2: break
			count += 1
			sum += len(self.good[n][2])
			if prn: 
				data += self.good[n][2]+'\n'
			n += 1

		return n, count, sum, data

			
	def analyze2(self):
		pos = 0
		roots = []
		while pos < len(self.good):
			info = self.good[pos]

			pos, count, sum, data = self.find_group(pos, prn=1)
			if sum/count > 50:
				print '%10s %2d %3d' % ( info[0], info[1], len(info[2]) ), info[2][:60]
				print '%d items, AvgLen= %d\n' % (count, sum/count)
				print '-'*50,'\n', data, '-'*50

	def prn_anchors(self):
		for url,ref in self.anchors:
			print "%s -- [%s]" % (url, ref)
