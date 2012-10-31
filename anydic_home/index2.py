#!/bin/env python
# -*- coding: EUC-KR -*-

import adtemplate
import adstatic

if __name__=='__main__':

	print 'Content-Type: text/html\n'

	WORD = adstatic.randWord()
	keywords = "번역, 영화, DVD, 서버, 광랜,  랜카드, 초고속 인터넷, 위성 방송, DMB, 학원, 수능 방송 "

	adtemplate.adHead(keywords, "애니딕: 컴퓨, 인터넷 " )
	adtemplate.adLogo()
	adtemplate.adForm("ad.py", WORD)
	adtemplate.adGoogle()


	adstatic.adRecent()

	adstatic.adFooter()

