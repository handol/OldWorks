#!/usr/bin/env python
# -*- coding: EUC-KR -*-

import os
import sys
import stattestclient


def read_stat_cfg(cfg_fname):
	try:
		fd = open(cfg_fname, 'r')
	except:
		print "reading failed: %s" % cfg_fname
		return []

	tests = []
	for line in fd:
		line = line.strip()
		line = line.lstrip('#')
		line = line.strip()
		flds = line.split()
		if len(flds) < 3: continue
		if flds[0]!='U' and flds[0]!='D' and flds[0]!='M': continue

		tests.append(flds)
	return tests

def nomalizeUrl(url):
	url = url.lstrip('http://')
	url = url.rstrip('/')
	return url

def do_test(cfg_fname, port_num=8080, num=0):
	tests = read_stat_cfg(cfg_fname)
	if len(tests)==0: return

	tests.append(["U", "1", "http://한글url"])
	cnttest = {}
	for test in tests:
		url = nomalizeUrl(test[2])
		try:
			cnttest[url] += 1
		except:
			cnttest[url] = 1
		
	print "## %d cases" % len(tests)
	print "## %d unique cases" % len(cnttest)
	
	#for k,v in cnttest.iteritems():
	#	print k, v

	#for test in tests:
	#	print "%s %s %s" % (test[0], test[1], test[2])
		

	print "\n====================="
	n = 0
	failed = []
	for test in tests:
		print "==="
		resdata = stattestclient.do_main(HOST='localhost', PORT=port_num, URL=test[2], MDN='0167000258')
		pos = resdata.find("HTTP/1")
		outstr = ''
		if pos != -1:
			pos2 = resdata.find("\n", pos)
			if pos2 != -1:
				outstr = resdata[pos:pos2]

		flds = outstr.split()
		if len(flds) >= 2: res_code = int(flds[1])
		else: res_code = 0

		if res_code != 299:  
			failed.append([test, res_code])
 
		n += 1

		#if n == (len(tests) / 4):
		#	os.system("touch %s" % cfg_fname)

		print "[%d] %s %s %s" % (n, test[0], test[1], test[2])
		print "[%d] CODE=%d result leng=%d" % (n, res_code, len(resdata))
		pos = resdata.find("\n\n")
		if pos < 0: pos = resdata.find("\r\n\r\n")
		if pos > 0: print resdata[pos:].strip()
		if num != 0 and n == num: break

	print "\n==== NOT Blocked: printing RES CODE"
	for fail in failed:
		test = fail[0]
		print "%s %s %s --> %d" % (test[0], test[1], test[2], fail[1])


if __name__=="__main__":
	if len(sys.argv) < 3:
		print "%s stat_cfg_file pas_port [num_test]" % (sys.argv[0])

	elif len(sys.argv) > 3:
		do_test(sys.argv[1], int(sys.argv[2]), int(sys.argv[3]) )
	else:
		do_test(sys.argv[1], int(sys.argv[2]) )

