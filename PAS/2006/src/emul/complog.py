#!/usr/bin/env python
# -*- coding: EUC-KR -*-

import sys

class DictWithList(dict):
	'''
	 중복되는 key들의 value를 list 형태로 저장하는 dict.

	 중복이 가장 많은 key 순으로 출력한다.
 	'''

        # key, value를 dict 에 추가하는 것.
	def add(self, key, value):
		try:
			self[key].append(value)
		except:
			self[key] = [value]

        # 중복이 가장 많은 key 순으로 출력한다.
	def rank(self):
		# rank high those which has high counter value
		#return sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)
		return sorted(self.iteritems(), key = lambda x: len(x[1]), reverse=True)

	def prn(self, mincnt=0):
		# rank high those which has high counter value
		ranked = sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)
		n = 0
		for (key, vals) in ranked:
			if len(vals) > mincnt:
				n += len(vals)
				print "%4d\t%s" % (len(vals), key)
		print "# total: %d" % (n)

	def html(self, mincnt=0):
		# rank high those which has high counter value
		ranked = sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)
		print "<table>"
		for (key, vals) in ranked:
			if len(vals) > mincnt:
				print "<tr><td>%4d</td> <td>%s</td></tr>" % (len(vals), key)
		print "</table>"

def compLine(line1, line2, indexlist):	
	'line 2개를 받아 서로 비교한다'

	a = line1.split()
	b = line2.split()
	if len(a) != len(b):
		print 'length error'
		return 0	# if not same

	for idx in indexlist:
		if a[idx] != b[idx]:
			return 0	

	return 1	# if same

def compUrl(line1, line2, indexlist, option):
	'URL 중심으로 비교한다'

	a = line1.split()
	b = line2.split()

	if len(a) != len(b):
		print 'length error'
		return 0	# if not same

	if option == '-s':
		urlinx = 12
	elif option == '-m':
		urlinx = 15

	if a [ urlinx ] == b [ urlinx ]:
		for idx in indexlist:
			if a [ idx ] != b [ idx ]:
				print
				print "<Different Result>"
				print "    PhoneNumber: %s" % a [ 2 ], 
				print "SameUrl: %s" % a [ urlinx ]
				print "    <Oldfile Line %s>" % a [0].split('$') [0],
				print a [ idx ] 
				print "    <Newfile Line %s>" % b [0].split('$') [0],
				print b [ idx ] 
				return 0
	else: 
		return 0

	return 1

def addtodict(filename1, filename2):
	'파일 2개를 받아 dict에 저장한다'

	fp1 = open(filename1, 'r')
	fp2 = open(filename2, 'r')

	lineNumA = 1
	lineNumB = 1

	for line in fp1:
		dwlist1.add(line.split()[2], repr(lineNumA) + '$' + line)
		lineNumA += 1

	for line in fp2:
		dwlist2.add(line.split()[2], repr(lineNumB) + '$' + line)
		lineNumB += 1
	#dwlist.prn()
	fp1.close()
	fp2.close()

def do_it():
	pass

def sortDict(adict):
	keys = adict.keys()
	keys.sort()

	return map(adict.get, keys)

def compLines(phoneNum, oldLines, newLines, kindOfInx):
	sizeA = len(oldLines)
	sizeB = len(newLines)
	sizeM = max(sizeA, sizeB)

	a = 0
	b = 0
	matchCnt = 0

	if kindOfInx == '-s':
		Inx = statInxList
	elif kindOfInx == '-m':
		Inx = moneyInxList

	while True:	
		#rz = compLine(oldLines[a] , newLines[b], Inx)
		#assert a != sizeA, 'ho'
		rz = compUrl(oldLines[a] , newLines[b], Inx, kindOfInx)

		if rz == 1:
			matchCnt += 1
			a += 1
			b += 1
			# DEBUG
			if b == sizeB:
				b = 0
		elif rz == 0:
			b += 1
			
			if b == sizeB:	# modified by clark
				a += 1
				b = 0
				
		if a >= sizeA: break
		#if b >= sizeB: break	# modified by clark

	print "-" * 70
	print "%s : match %d Lines | Old : %d Lines | New : %d Lines Exist" % (phoneNum, matchCnt, sizeA, sizeB)
	print "-" * 70
	

dwlist1 = DictWithList()
dwlist2 = DictWithList()

# Field Definition
# 0 -> First Field
# 1 -> Second Field
statInxList = [ 2, 3, 8, 9, 10, 11, 12 ]
moneyInxList = [ 1, 2, 5, 7, 8, 10, 13, 15] # Catch Field

if __name__ == '__main__':

	# TO DO
	# ADD OPTION when argument is just 1

	if len(sys.argv) != 4:
		print '-' * 50
		print 'Status   Log: -s'
		print 'Money    Log: -m'
		print "Usage: complog.py -s[m] file1 file2"
		print '-' * 50
		sys.exit(0)

	if sys.argv[1] != '-s' and sys.argv[1] != '-m':
		print 'Use -s or -m'
		sys.exit(0)

	print '-' * 70
	print ' Welcome To Log Compare Tool'
	print '-' * 70
		
	addtodict(sys.argv[2], sys.argv[3])
	
	lists1 = dwlist1.keys()
	lists1.sort()

	for phone in lists1:
		if not dwlist2.has_key(phone):
			print "%s Not found phoneNumber" % phone
			print "-" * 70
		else:
			compLines(phone, dwlist1[phone], dwlist2[phone], sys.argv[1])

	#print dwlist1


