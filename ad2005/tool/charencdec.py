#!/usr/bin/env python
# -*- coding: EUC-KR -*-

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


hanstr = '가나다'
a = unicode(hanstr, 'euc-kr')
b = a.encode('euc-kr')
prnhex( hanstr )
prnhex( a )
prnhex( b )
