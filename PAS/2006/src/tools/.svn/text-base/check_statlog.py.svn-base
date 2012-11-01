#!/usr/bin/python

import sys, re

def countStr(arr, sstr):
	i = 0
	for parr in arr:
		if parr.lower().startswith(sstr): i += 1
	return i

def locateStr(arr, sstr):
	i = 0
	j = 0
	while i < len(arr):
		i += 1
		if arr[-i].startswith(sstr): j += 1
		if j == 2: return -i
	return 0


try:
	f = open(sys.argv[2])
except:
	print "Usage: pas2stat.py option logfilename"
	sys.exit()

realStr = ''
old_i = []
for lineStr in f:
	lineStr = lineStr.strip()	
	realStr += ' '+lineStr
	realStr = realStr.strip()
	szTemp = re.search('[0-9]{4}/[0-9]{2}/[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}', realStr[5:])
	if szTemp:
		doStr = realStr[0:szTemp.start()+5]
		splitStr = doStr.split()
		if len(splitStr) == 12 or len(splitStr) == 13 or countStr(splitStr, 'http') == 1:
	   		if sys.argv[1] == '-all': print doStr
		elif countStr(splitStr, 'http') == 0:
			old_i.append(splitStr)
		else:
			if len(old_i) == 0:
				if sys.argv[1] == '-test': print 'test1', splitStr
			else:
		  	 	i = 0
				cnt = countStr(splitStr, 'http') - 1
				while i < cnt:
					i += 1
					try: now_i = old_i.pop()
					except: break
					j = locateStr(splitStr, 'http') + 1
					while j < 0:
						now_i.append(splitStr[j])
						j += 1
					pStr = ''
					for tempStr in now_i: pStr += ' '+tempStr.strip()
					if len(now_i) in (12, 13) and (sys.argv[1] == '-repair' or sys.argv[1] == '-all'): print pStr.strip()
					elif len(now_i) not in (12, 13) and sys.argv[1] == '-test': print 'test2', now_i
					splitStr = splitStr[0:locateStr(splitStr, 'http')+1]
					
				pStr = ''
				for tempStr in splitStr: pStr += ' '+tempStr.strip()
				if len(splitStr) in (12, 13) and (sys.argv[1] == '-repair' or sys.argv[1] == '-all'): print pStr.strip()
				elif len(splitStr) not in (12, 13) and sys.argv[1] == '-test': print 'test3', splitStr

		realStr = realStr[szTemp.start()+5:].strip()
f.close()
