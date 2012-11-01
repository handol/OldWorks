#!/usr/bin/env python

import sys

phoneTable = {}

def makeTable(filename, fieldNum):
	fp = open(filename, 'r')
	for line in fp:
		splitline = line.split()
		if phoneTable.has_key(splitline[ fieldNum - 1 ]):
			phoneTable[ splitline [ fieldNum - 1 ]].append(line)
		else:
			phoneTable[ splitline [ fieldNum -1 ]] = [ line ]

	fp.close()

def writeFile(extention):
	for key in phoneTable.keys():
		fpw = open(key + '.' + extention, 'w')
		phoneTable.get(key).reverse()

		while True:
			fpw.write(phoneTable.get(key).pop())
			if len(phoneTable.get(key)) == 0:
				break
		fpw.close()

if __name__ == '__main__':

	if len(sys.argv) != 4:
		print 'usage: splitFileWithField.py input_file_name field_number extention_name_of_outfile'
		sys.exit(0)
	

	makeTable(sys.argv[1], int(sys.argv[2]))
	writeFile(sys.argv[3])

