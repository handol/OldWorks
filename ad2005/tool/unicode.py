#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# http://diveintopython.org/xml_processing/unicode.html -- python°u unicode, encoding
import sys
import codecs
import os

#sys.setdefaultencoding('EUC-KR')
print sys.getdefaultencoding()


def prnhex(str):
	print "---"
	print str
	for c in str:
		print "%02X" % ord(c)
	print "---"

(e,d,sr,sw) = codecs.lookup('euc-kr')


a = u"hello"
prnhex(a)

b = "hello"
prnhex(b)


class StreamRewriter(codecs.StreamWriter):

	encode = e
	decode = d

	def write(self,object):

		""" Writes the object's contents encoded to self.stream
			and returns the number of bytes written.
		"""
		data,consumed = self.decode(object,self.errors)
		self.stream.write(data)
		return len(data)


fd = open('/home/dahee/data/engtokor_wd_lis_unicodet.txt')

cnt=0
for line in fd.xreadlines():
	cnt = cnt + 1
	if cnt==1: continue
	#l = unicode(line).encode('euc-kr')
	l = line.decode('utf-16').encode('euc-kr')
	print cnt, l
	if cnt > 3: break
	
