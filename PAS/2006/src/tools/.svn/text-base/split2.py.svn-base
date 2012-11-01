#!/usr/bin/env python
import sys

def getflds(fname, indexlist, delimiter=None, do_strip=False):
	if fname=='-':
		fd = sys.stdin
	else:
		try:
			fd = open(fname, 'r')
		except:
			print "Read fail: ", fname
			return

	if delimiter != None:
		do_strip = True

	indexlist.sort()
	maxidx = indexlist[-1]

	for line in fd:
		flds = line.split(delimiter)
		if len(flds) < maxidx + 1:
			continue

		if do_strip:
			flds = map(str.strip, flds)
		reslist = []
		for i in indexlist:
			reslist.append(flds[i])
		resline = " , ".join(reslist)
		print resline
		


if __name__=="__main__":
	if len(sys.argv) < 3:
		print "usage: filename field_list(comma-separated) delimiter(default=space)"
		sys.exit()

	fldsList = sys.argv[2].split(',')
	fldsList = map(int, fldsList)
	if len(sys.argv) > 3:
		delimiter = sys.argv[3]
	else:
		delimiter = None
	
	getflds(sys.argv[1], fldsList, delimiter)
