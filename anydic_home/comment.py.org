#!/usr/local/bin/python
# -*- coding: EUC-KR -*-

import adtemplate
import adstatic

def prnFile(fname):
	try:
		fd = open(fname, 'r')
	except:
		return

	print fd.read()

if __name__=='__main__':

	print 'Content-Type: text/html\n'

	WORD = adstatic.randWord()
	keywords = "특허, 아이디어, 포토샵, 디자인, 메일 서버 "

	adtemplate.adHead(keywords, "애니딕 제안,소감, 아이디어, 디자인 " )
	adtemplate.adLogo()
	adtemplate.adForm("ad.py", WORD)
	adtemplate.adGoogle()

	prnFile("cmnt.html")


	adstatic.adFooter()

