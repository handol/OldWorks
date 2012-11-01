#!/usr/bin/python

import sys

try:
	f = open(sys.argv[1])
except:
	print "Usage: get_time.py filename time(interval)"
	sys.exit()



skipbit = 0
dataDic = {}
for lineStr in f:
    lineStr = lineStr.strip()
    splitStr = lineStr.split(' ')
    if lineStr == '' or len(splitStr) < 4: continue
    try: dataDic[splitStr[0]][int(splitStr[1])-1] = splitStr[3]
    except:
        dataDic[splitStr[0]] = ['0', '0']
        dataDic[splitStr[0]][int(splitStr[1])-1] = splitStr[3]
f.close()

total = 0
count = 0
total2 = 0
count2 = 0
dif = 0
for key in dataDic.keys():
    row = dataDic[key]
    if row[0] != '0' and row[1] != '0':
        dif = float(row[1]) - float(row[0])
        if dif > 0 and dif <= int(sys.argv[2]):
            count += 1
            total += dif
            print count, total, dif
        else:
            count2 += 1
            total2 += dif
            print count, total, dif
print total, count, total/count, total2, count2, total2/count2
