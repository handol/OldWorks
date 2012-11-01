#!/usr/bin/python

import sys

def do_it(filename):
	fp = open(filename, 'r')

	data = fp.read()
	data = data.split('===')

	for article in data:
		term = article.split()
		#print term[0], 'No problem'
		#if term[0] == 'Blocking':
		if term.count('Blocking') > 0 and term.count('HTTP/1.1') > 0:
			try:	
				term.index('299')
			except:
				print 'Blocking: not 299 status'
				print article
				print '-' * 15
		elif term.count('Non-Blocking') > 0 and term.count('HTTP/1.1') > 0:
			try:
				term.index('302')
			except:
				print 'Non-Blocking: not 302 status' 
				print article
				print '-' * 15

	fp.close()

	#for line in fp:
#		nowblock = 0
#		if line.find('==='):
#			block = line.split() [1]
#			if block == 'Blocking':
#				nowblock = 1
#			elif block == 'Non-Blocking':
#				nowblock = 2
#			else:
#				print 'Error Blocking'
#		elif line.find('HTTP/1.1

			


if __name__ == '__main__':
	if len(sys.argv) != 2:
		print 'Use: python verifyGongji.py filename'
		sys.exit(0)
	
	do_it(sys.argv[1])


