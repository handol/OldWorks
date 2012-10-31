#!/usr/bin/env python
# -*- coding: EUC-KR -*-

def proc_webster(dictfile, out=None):
	cnt = 0

	try:
		fd = open(dictfile)
	except:
		print "read fail:", dictfile
		retun
	
	html = fd.read()

	ptr = 0
	prev = ''
	while 1:
		p1 = html.find('<P>', ptr)
		if p1==-1: break
		p2 = html.find('</P>', p1)
		if p2==-1: break

		p3 = html.find('<B>',ptr)
		if p3==-1: break
		p4 = html.find('</B>',ptr)

		ptr = p4+4

		w = html[p3+3:p4]
		if html[p4+4].isspace():
			expl = html[p4+5:p2]
		else:
			expl = html[p4+4:p2]
	
		if out != None: 
			if w != prev:
				out.write('\n')
				out.write(w)
				out.write('\n')
				prev = w
			out.write(expl)
			out.write('\n')

		cnt += 1
	
	print "Total:", cnt
 	
if __name__ == "__main__":
	import time
	import sys

	t = time.time()

	out = None
	if len(sys.argv) > 1:
		try:
			out = open(sys.argv[1], "w")
		except:
			print "cannot write:", sys.argv[1]
	
		proc_webster('/data1/AD/data/v003/wb1913_a.html', out)

	print "WordNet %.3f" % (time.time()-t)


