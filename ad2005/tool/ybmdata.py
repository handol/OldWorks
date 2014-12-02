#!/usr/bin/env python
# -*- coding: EUC-KR -*-
import sys
import codecs
import os

DATA_DIR = '/home/dahee/AD2005_data/'
ENG_DICT = DATA_DIR + 'engtokor_wd_lis_wansung.txt'
ENG_EXPL = DATA_DIR + 'engtokor_expn_list_wansung.txt'

OUT_IDIOM = DATA_DIR + 'sisa_idiom.dict'
OUT_ENG = DATA_DIR + 'engword.out'
OUT_KOR = DATA_DIR + 'korword.out'

idiom_fd = None
engword_fd = None
korword_fd = None

## python dict(map) type
# key == wd_seq_num
# value == [단어, 난이도, 품사목록 ]
EXPL_LIST = {}

def prnhex(str):
	print "---"
	print str
	for c in str:
		print "%02X" % ord(c)
	print "---"


def try_open(fname, mode='r'):
	try:
		fd = open(fname, mode)
		return fd
	except:
		print 'open failed:', fname
		return None
	

# 영어 사전 설명 파트에서 한글이 나오기 전까지의 설명 내용을 return
# input: 영어 사전 설명 내용 
# output: 하늘 나오기 전까지 내용.
def eng_expl_remove_hangul(str):
	n = 0
	for ch in str:
		if ord(ch) & 0x80: break
		n = n + 1
	
	if n < 3: return ''
	if n >= len(str)-1: return str[:n]

	prev_ch = str[n-1]
	if prev_ch.isupper() or prev_ch=='(':
		n = n - 1

	res = str[:n].strip()
	
	ch  = res[-1]
	if ch=='!' or ch=='?':
		return ''
	return res

def eng_meaning(explfile, debug=0):
	fd = try_open(explfile)

	cnt=0
	for line in fd.xreadlines():
		cnt = cnt + 1
		flds = line.split('||')
		if len(flds) < 5: continue
		
		try:
			wd_seq_num = int(flds[1])
			flag = int(flds[3])
		except:
			continue

		try:
			word_info = EXPL_LIST[wd_seq_num]
		except:
			print "not found:", wd_seq_num
			continue

		# flag: 2- 단어 설명, 4 - 예문
		if flag==1: ## 품사
			word_info[2].append( int(flds[4]) )
		elif flag==3: ## 숙어
			idiom = eng_expl_remove_hangul( flds[4] )
			
			if debug:
				print flds[4], idiom
				print
			if idiom != '':
				idiom_fd.write( word_info[0] )
				idiom_fd.write( ' : ' )
				idiom_fd.write( idiom )
				idiom_fd.write('\n')	

	idiom_fd.close()



def eng_dict(dicfile):
	fd = try_open(dicfile)

	cnt=0
	for line in fd.xreadlines():
		cnt = cnt + 1

		flds = line.split('||')
		if len(flds) < 6: continue
		
		try:
			wd_seq_num = int(flds[0])
		except:
			continue

		wd_level = int(flds[5])

		try:
			EXPL_LIST[wd_seq_num] = [flds[2], wd_level, [] ]
		except:
			print "fail:", flds[2], wd_level
		
		#if cnt > 100: break


'''
aa = "get the 411 on 한국 하하 "
for ch in aa:
	print ch, ord(ch)
	if ord(ch) & 0x80: break
'''

if __name__=='__main__':
	do_flag = 0
	#sys.exit()

	if len(sys.argv) > 1 and sys.argv[1]=="make":
		do_flag = 1

	idiom_fd = try_open( OUT_IDIOM, 'w' )

	eng_dict ( ENG_DICT )
	eng_meaning ( ENG_EXPL )

	if do_flag:
		for val in  EXPL_LIST.iteritems():
			val[1][2].sort()
			PoSlist = map(str, val[1][2])
			PoS = ','.join(PoSlist)
			#print '%d|%s|%d|%s' % (val[0], val[1][0], val[1][1], PoS)
			print '%s|%d|%s' % (val[1][0], val[1][1], PoS)
			

# how to: python ybmdata.py make > sisa_word.dict
