#!/usr/bin/env python
# -*- coding: EUC-KR -*-

import sys
import os
import time

Chr = ['U', 'D', 'M']

def make_sharp():
	'마지막줄 U를 #U로 만들어 준다'

	fp = open('k_stat.cfg', 'r+')
	fw = open('tmpwrite.cfg', 'w')

	did = False
	theurl = -1

	for line in fp:
		#if line[0] == 'U' and did == False:
		if Chr.count(line[0]) > 0 and did == False:
			line = '#' + line
			did = True
			theurl = line.split()[2]

		fw.write(line)
	if did == 'False':
		print '*' * 50
		print 'Not found U'
		print '*' * 50

	fp.close()
	fw.close()
	os.system('mv -f tmpwrite.cfg k_stat.cfg')
	return theurl

def current_url():
	'마지막줄 U의 url을 return한다'
	fp = open('k_stat.cfg', 'r')
	for line in fp:
		#if line[0] == 'U' or line[0] == 'D' or line[0] == 'M':
		if Chr.count(line[0]) > 0:
			return line.split()[2]

	return -1

def del_sharp():
	'만드는 중'

	fp = open('test.cfg', 'r+')
	fd = open('tmpdel.cfg', 'w')

	ready = False
	for line in fp:
		if line[ 0 : 2 ] == '#U' and ready == False:
			pass


if __name__ == '__main__':
	cmd = 'python simplekun.py 5400 0160001234 '
	print 'Start'
	#print 'Caution!! This Program is very very specified Current k_stat.cfg'

	err_cnt = 0
	err_mesg = ''
	while True:

		print '-' * 50
		print 'Before Do #'

		nowurl = current_url()

		if nowurl == -1:
			print 'No more U or M or D'
			print 'Wrong Case:', err_cnt
			print err_mesg
			sys.exit(0)
		else:
			if nowurl.find('http') == -1:
				nowurl = 'http://' + nowurl
			print 'nowurl: %s' % nowurl

			nowurl = "'" + nowurl + "'"
			res = os.popen(cmd + nowurl)
			data = res.read()
			data = data.split()
			try:
				inx = data.index('HTTP/1.1')
			except:
				print 'NO HTTP RESPONSE'
			print data[inx : inx + 2]

			if data[inx + 1] != '299':
				err_cnt = err_cnt + 1
				err_mesg = err_mesg + nowurl + '\n'


			print '-' * 50
			print 'After Do #'

			afterurl = make_sharp()

			print 'Pas configure take a few seconds'
			time.sleep(6)
			if afterurl.find('http') == -1:
				afterurl = 'http://' + afterurl
			print 'afterurl: %s' % afterurl
			afterurl = "'" + afterurl + "'"
			res = os.popen(cmd + afterurl)
			data = res.read()
			data = data.split()
			try:
				inx = data.index('HTTP/1.1')
			except:
				print 'NO HTTP RESPONSE'
			print data[inx : inx + 2]



