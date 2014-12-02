#!/usr/bin/env python
# -*- coding: EUC-KR -*-
# http://diveintopython.org/xml_processing/unicode.html -- python°u unicode, encoding
import sys
import os

#sys.setdefaultencoding('EUC-KR')
print sys.getdefaultencoding()


def prnhex(str):
	print "---"
	print str
	for c in str:
		print "%02X" % ord(c)
	print "---"

fd = open('/home/dahee/data/engtokor_wd_lis_unicodet.txt')

for line in fd.xreadlines():
	#l = unicode(line).encode('EUC-KR')
	#l = line.encode('EUC-KR')
	prnhex (line)
	break
