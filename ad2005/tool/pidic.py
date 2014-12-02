#!/usr/bin/env python
# -*- coding: EUC-KR -*-
#
# read a html file of a given url, and find *.jpg file and save the image
# http://www.pidic.com/dictionary/definition/good.html

import os
import re
from urllib import urlopen

### input html

#<BR><BR><B>'good' as a noun:</B><BR><BR>
#<BR><IMG SRC="http://www.pidic.com/images/04869096.jpg" ALT="good"><BR><BR>


### output
#  noun , 04869096.jpg


## 주어진 html 문에서 단어의 품사와 이미지 파일명을 구한다.

def getPoSandImage(htmlstr, debug=0):
	if debug: print htmlstr
	p = re.compile(r'as an? (\w+)', re.M)
	poomsa = p.search(htmlstr)

	p = re.compile(r'\d+\.jpg', re.M)
	imgname = p.search(htmlstr)

	if imgname != None:
		return poomsa.group(1) , imgname.group()
	else:
		return None, None
		


####
# pidic.com 에서 주어진 단어에 대한 image file 명과 품사를 구한다.
# good 이란 단어 사례로 commnet 작성한다.
def	fetchPidic(word, debug=0):
	result = []

	urlstr = "http://www.pidic.com/dictionary/definition/%s.html" % word
	
	try:
		doc = urlopen(urlstr).read()
		#if debug: print doc
	except:
		print "fetch failed for ", word
		return []

	target = "'%s'" % word
	
	while 1:
		# find 'good'
		pos = doc.find(target)
		if pos == -1: break

		poomsa,imgname = getPoSandImage (doc[pos:pos+127] )

		if imgname != None:
			result.append( (imgname, poomsa) )
			print "%10s %s %s" % (word, imgname, poomsa)

		doc = doc[pos+128:]
	return result

WGET = os.popen("which wget", 'r').readlines()[0].strip()
CONVERT = os.popen("which convert", 'r').readlines()[0].strip()

def downloadImg(imgurl, imgname):
	try:
		imgdata = urlopen(imgurl).read()
	except:
		return 0

	try:
		outfd = open(imgname, "wb")
		outfd.write(imgdata)
		outfd.close()
	except:
		print "failed writing img file: ", imgname
		return 0

	return 1
	
def saveImage(outfd, word, imglist):
	for img in imglist:
		## wget
		imgurl = "http://www.pidic.com/images/%s" % img[0]
		imgname = "%s-%s.jpg" % (word, img[1])

		cmd = "%s -q -o %s %s" % (WGET, imgname, imgurl)
		#print cmd
		#os.popen(cmd, 'r').readlines()

		res = downloadImg( imgurl, imgname)
		if res == 0:
			print "img download failed :", word
		

		## convert
		cmd = "%s -quality 70 %s %s" % (CONVERT, imgname, imgname)
		print cmd
		os.popen(cmd, 'r').readlines()

		## write img name to a file
		outfd.write("%s %s\n" % (word, imgname) )


def isalphaonly(str):
	for ch in str:
		if not ch.isalpha(): return 0
	return 1

def getWordList(dictfile, outfile):
	try:
		fd = open(dictfile, 'r')
	except:
		print "read failed: ", dictfile
		return

	try:
		out = open(outfile, 'a')
	except:
		print "writing failed: ", outfile
		return
		
	for line in fd.xreadlines():
		if not line[0].isalpha(): continue

		str = line.split(':')[0]
		word = str.split()
		if len(word) > 1: continue
		if not isalphaonly(word[0]): continue

		word = word[0]
		print word

		imglist = fetchPidic (word)
		if imglist != []:
			saveImage(out, word, imglist)
			break		

	fd.close()
	out.close()
			
if __name__ == "__main__":
	import sys
	if len(sys.argv) > 1: 
		word = sys.argv[1]	
		fetchPidic(word, debug=1)
	#else: 
	#	sys.exit(0)


	getWordList( "/data1/AD/data/word.all", "pidic.out")
