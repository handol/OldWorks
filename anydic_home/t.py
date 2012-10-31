#!/usr/local/bin/python

import urllib

print 'Content-Type: text/html\n'

doc = urllib.urlopen("http://stock.naver.com").read()
print doc
#print 'dahee zzang !!'


