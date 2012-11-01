#!/usr/bin/env python


import os
import sys
from operator import itemgetter


###

###
class DictWithList(dict):
	def add(self, key, value):
		try:
			self[key].append(value)
		except:
			self[key] = [value]

	def rank(self):
		# rank high those which has high counter value
		return sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)

	def prn(self, mincnt=0):
		# rank high those which has high counter value
		ranked = sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)
		n = 0
		for (key, val) in ranked:
			if len(val) > mincnt:
				n += len(val)
				print "%4d\t%s" % (len(val), key)
		print "# total: %d" % (n)

	def html(self, mincnt=0):
		# rank high those which has high counter value
		ranked = sorted(self.iteritems(), key=lambda x: len(x[1]), reverse=True)
		print "<table>"
		for (key, val) in ranked:
			if len(val) > mincnt:
				print "<tr><td>%4d</td> <td>%s</td></tr>" % (len(val), key)
		print "</table>"


def get_mem_cnt(dictitem):
	return dictitem[1][4]

def get_item_cnt(dictitem):
	return dictitem[1][5]
###
# data := begin_time  + end_time + alloc/free + mem_addr + freq + itemCount
# alloc == +1
# free == -1
class DictWithCnt(dict):
	def __init__(self):
		dict.__init__(self)
		self.itemCount = 0
	def add(self, data, cnt=0):
		self.itemCount += 1
		data[5] = self.itemCount
		
		key = data[3] # mem address
		try:
			in_data = self[key]
			in_data[1] = data[1]
			in_data[2] += data[2]
			in_data[4] += data[4]
		except:
			self[key] = data


	def rank(self):
		# rank high those which has high counter value
		return sorted(self.iteritems(), key=get_mem_cnt, reverse=True)

	def prn(self, mincnt=0):
		# rank high those which has high counter value
		ranked = sorted(self.iteritems(), key=get_mem_cnt, reverse=True)

		for (key, val) in ranked:
			if val[1] > mincnt:
				print val

	def info(self, mincnt=0):
		ranked = sorted(self.iteritems(), key=get_item_cnt, reverse=False)
		cnt = 0
		for (key, val) in ranked:
			if val[4] < mincnt:
				cnt += 1
				#print "%s %s %d %d" % ( val[0], val[2], val[1], val[3])
				print val
	
		print "Printed = %d" % (cnt)
		print "All = %d" % (len(self))

###
def load(fname):
	d = DictWithCnt()

	try:
		fd = open(fname, 'r')
	except:
		return

	for  line in fd:
		flds = line.split()
		if len(flds) < 5: 
			print "ODD -- ", line	
			continue

		tmval = flds[0][:8]
		if flds[1].endswith("Alloc"):
			cnt = 1
		elif flds[1].endswith("Free"):
			cnt = -1

		if flds[4].startswith("MB"):
			memaddr = flds[4][3:-1]
		data = [tmval, tmval, cnt, memaddr, 1, 0]
		d.add(data)

	
	d.info(10)

if __name__ == "__main__":
	if len(sys.argv) > 1:
		load(sys.argv[1])
