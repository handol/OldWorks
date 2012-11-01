#!/bin/env python

import sys

def cntLinesWithSamePrefix(fname, len_prefix):
	try:
		fd = open(fname, 'r')
	except:
		print "file open fail:", fname
		return
	cnt = 0
	for prevline in fd:
		prevline = prevline[:len_prefix]
		cnt += 1
		break

	print prevline

	for line in fd:
		thisline = line[:len_prefix]
		if prevline != thisline:
			print "%s\t%d" % (prevline, cnt)
			cnt = 1
			prevline = thisline
		else:
			cnt += 1
		
	print "%s\t%d" % (prevline, cnt)
	
if __name__ == "__main__":
	if len(sys.argv) >= 3:
		cntLinesWithSamePrefix(sys.argv[1], int(sys.argv[2]))		
	else:
		print "usage: file_name leng_of_prefix"
		sys.exit()
 
