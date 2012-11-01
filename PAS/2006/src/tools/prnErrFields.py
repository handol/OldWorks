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

def printField(filename, separator=' '):
	'result 를 바탕으로 특정 field를 프린트한다'

	try:
		fp = open(filename, 'r')

	except:
		print 'No exist file'
		sys.exit(0)

	lineCnt = 0
	errLineCnt = 0
	for line in fp:
		lineCnt += 1
#		line = line[ : -1 ] ####
		if separator == ' ':
			splitedline = line.split()
		else:
			splitedline = line.split(separator)

		output = ""

		#print 'splitline:', splitedline

		tmp = len(result)

		if len(splitedline) < result[ tmp - 1 ] and result[ tmp - 1 ] > 0:
			errLineCnt += 1
			#print line
		continue

		for num in result:

			# join is faste is faster

			if num < 0:
				output = output + ' ' + splitedline[ num ]
			else:
				output = output + ' ' + splitedline[ num - 1 ]

		print output

	print "### error lines = %d" % (errLineCnt)
	print "### lines = %d" % (lineCnt)

	fp.close()

if __name__ == '__main__':


	if len(sys.argv) == 3:
		dealArg(sys.argv[2])
		printField(sys.argv[1])

	elif len(sys.argv) == 4:
		dealArg(sys.argv[2])
		printField(sys.argv[1], sys.argv[3])

	else:	
		print 'Usuage: python prnFields.py input_file list_of_fields [separator]'
		sys.exit(0)



