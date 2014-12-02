#!/usr/bin/env python
import sys
import os

def hexprn_file(file):
	fd = open(file)
	for line in fd.readlines():
		hexstr = map(hex, map(ord, line[:-1]) )
		#print hexstr
		print " ".join(hexstr)
		
if __name__=='__main__':
	if len(sys.argv) > 1:
		hexprn_file(sys.argv[1])
