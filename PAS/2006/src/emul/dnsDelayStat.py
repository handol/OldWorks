#!/usr/bin/env python
## -*- coding: EUC-KR -*-

# '필드 출력기'
# 2006. 11.21


import sys

result = []

def dealArg(field):
	'argv를 처리하여 field number 를 return 해준다.'


	#if field.find(',') != -1:

	newf = field.split(',')
	length = len(newf)

	for seq in newf:
		if seq.find('-') != -1 and seq.find('-') != 0:	# 만약 5-8 이 있다면 
								# -5, -8 이 아니라.
			result.extend(range(int(seq[ 0 ]), int(seq[ 2 ]) + 1))

		else:
			result.append(int(seq))

			
	#result.sort()	
	return result

def openFile(fname):
	if fname=="-":
		return sys.stdin
	else:
		try:
			fp = open(fname, 'r')
			return fp
		except:
			print 'cannot open file:', fname 
			raise
			return None 

def printDns(filename, separator=' '):
	'result 를 바탕으로 특정 field를 프린트한다'

	fp = openFile(filename)

	if fp==None:
		sys.exit(0)

	for line in fp:
		if line.find("DNS DELAY") !=-1:
			flds = line.split()
			print "%s , %s" % (flds[6], flds[4])


	fp.close()

if __name__ == '__main__':


	if len(sys.argv) == 2:
		printDns(sys.argv[1])

	else:	
		print 'Usuage: input_file'
		sys.exit(0)



