#!/bin/env python

import sys

def getFldValue(line, pos_fld):
	flds = line.split()
	try:
		return int(flds[pos_fld])
	except:
		return 0

def sumLinesWithSamePrefix(fname, len_prefix, pos_fld):
	try:
		fd = open(fname, 'r')
	except:
		print "file open fail:", fname
		return

	total = 0
	cnt = 0
	for line in fd:
		cnt += getFldValue(line, pos_fld)
		prevline = line[:len_prefix]
		break

	
	for line in fd:
		thisline = line[:len_prefix]
		if prevline != thisline:
			print "%s\t%d" % (prevline, cnt)
			total += cnt
			cnt = getFldValue(line, pos_fld)
			prevline = thisline
		else:
			cnt += getFldValue(line, pos_fld)
		
	print "%s\t%d" % (prevline, cnt)

	total += cnt
	print "\n%s\t%d" % ("Total:", cnt)
	
if __name__ == "__main__":
	if len(sys.argv) >= 4:
		sumLinesWithSamePrefix(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]))		
	else:
		print "usage: file_name leng_of_prefix pos_fields"
		sys.exit()
 
