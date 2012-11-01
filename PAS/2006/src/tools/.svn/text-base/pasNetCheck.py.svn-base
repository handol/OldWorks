#!/usr/bin/env python
## -*- coding: EUC-KR -*-

# '필드 출력기'
# 2006. 11.21


import sys

result = []

NET_2G = \
[ 53,  54,   55,   56,   62,   63 ]

def ipaddr2net(ipaddr):
	flds = ipaddr.split('.')
	net = map(int, flds)

	## 2.5G network
	if net[0]==10:
		if net[1] in NET_2G:
			return 1
	elif net[0]==61:
		if net[1]==252 and  net[2] in range(6,47):
			return 3

	return 0
	

HASH = {}
def insertHash(ipaddrstr):
	if HASH.has_key(ipaddrstr):
		return 0
	else:
		HASH[ipaddrstr] = 1
		return 1
	

def doit(filename, separator=' '):
	'result 를 바탕으로 특정 field를 프린트한다'

	cnt = [0]*4
	try:
		fp = open(filename, 'r')

	except:
		print 'No exist file'
		sys.exit(0)

	for line in fp:
		flds = line.split()
		if len(flds) < 11:
			continue
		if flds[2] == "[SSL]":
			continue
		if flds[3] != "-->":
			continue

		ipaddr = flds[10].split('/')[0]
		if ipaddr.count('.') != 3:
			print ipaddr
			continue

		if insertHash(ipaddr):
			network = ipaddr2net(ipaddr)
			cnt[network] += 1

	fp.close()

	print cnt
	print "total=", len(HASH)

if __name__ == '__main__':


	if len(sys.argv) == 2:
		doit(sys.argv[1])

	else:
		print 'Usuage: input_file'
		sys.exit(0)



