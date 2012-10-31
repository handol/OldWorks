#!/usr/local/bin/python

import urllib
import cgi
import sys

print 'Content-Type: text/html\n'

fields = cgi.FieldStorage()
try:
	code = fields["code"].value
except:
	code = ''
	print "code is missing"
	sys.exit(0)

urlstr = "http://stock.naver.com/sise/ksc_news.php?code=%s" % code

print "%s<br/>" % code
print "%s<br/>" % urlstr

print urllib.urlopen(urlstr).read()
#print 'dahee zzang !!'


