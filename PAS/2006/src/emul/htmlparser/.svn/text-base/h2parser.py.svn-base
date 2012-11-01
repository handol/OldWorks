#!/usr/bin/env python
import htmllib
import formatter

class myParser(htmllib.HTMLParser):
	def __init__(self, prn=0):
		if prn:
			format = formatter.AbstractFormatter(formatter.DumbWriter())
		else:
			format = formatter.NullFormatter()
			
		htmllib.HTMLParser.__init__(self, format)		
		self.depth = 0
		self.stack = []
			
	def handle_starttag(self, tag, method, attrs):
		self.depth += 1
		print ' '*self.depth + tag
		self.stack.append(tag)
		
	def handle_endtag(self, tag, attrs):		
		print ' '*self.depth + tag
		while 1:
			self.depth -= 1
			if self.stack[-1] != tag: 
				print '##', self.stack

			try:
				if self.stack.pop() == tag: break
			except:
				print '!!! tag mismatch'
				break

	def report_unbalanced(self, tag):
		print '???', tag
		self.depth -= 1
		
