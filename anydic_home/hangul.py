#!/usr/bin/env python
# -*- coding: EUC-KR -*-

HSUFFIX_ORG = [
"은", "는", "이", "가",
"을", "를", 
"의",
"스러운",
"하다", "주다", "있다", "없다", "가다", "오다", "시키다",
"만들다",
"하게", "한", "하여", "다운",
"으로", "된", "적인" 
]

def inverse(str):
	tmplist = list(str)
	tmplist.reverse()
	return ''.join(tmplist)

HSUFFIX = map(inverse, HSUFFIX_ORG)
HSUFFIX.sort()



def prnList(ws):
	for w in ws:
		print w


#prnList(HSUFFIX)

## 스트링에서 한글 단어만 추출하여 목록으로 제공.
def filterHanWords(line):

	hwords = []
	i = 0
	while i < len(line):
		start = i
		hlen = 0
		while i <len(line) and ord(line[i]) > 128 :
			val = (ord(line[i]) << 8) + ord(line[i+1])
			i += 2

			if val >= 0xB0A1 and val <= 0xC8FE:
				hlen += 2

		if hlen > 0:
			hwords.append(line[start:start+hlen])

		i += 1

	return hwords

## 한글 stemmer:어근 
## 조사, 어미 제거한 어근 찾기
## HSUFFIX 를 소팅하고, 문자열을 inverse 하여 속도 개선 가능.
def stemHanWord(word, debug=0):
	
	invword = inverse(word) ## 문자열을 뒤집기
	stems = [word]
	if len(word) < 6:
		return stems

	for s in HSUFFIX:
		if invword.startswith(s):
			stemlen = len(word) - len(s)
			if stemlen > 2:
				stems.append(word[:stemlen])
				break
		else:
			if cmp(invword, s) < 0: ## this means 'not found'
				break 

	if debug: prnList(stems)
	return stems


# 복합어 처리
#SPLIT = [
#(8, [ (4,4), (6,2), (2, 6)]),
#(10, [(4,6),(6,4),(4,4,2), (2,4,4)]),
#(12, [(4,4,4), (6,6), (4, 8), (8,4)]),
#(14, [(4,4,4,2), (4, 4 ,6), (4,6,4), (6,4,4)])
#]

SPLIT = [
(8, [ (4,4), (6,2)]),
(10, [(4,6),(6,4),(4,4,2)]),
(12, [(4,4,4), (6,6), (4, 8), (8,4)]),
(14, [(4,4,4,2), (4, 4 ,6), (4,6,4), (6,4,4)])
]

LENG = [s[0] for s in SPLIT]


def splitbylen(str, lens):
	res = []
	total = 0
	for l in lens:
		res.append( str[total:total+l] )
		total += l
	return res	

def stem_complex(word, debug=0):
	if not len(word) in LENG:
		return []

	lenslist = None
	for s in SPLIT:
		if s[0] == len(word):
			lenslist = s[1]
	
	if lenslist == None:
		return []

	stems = []
	for lens in lenslist:
		if lens > 2:
			stems += splitbylen(word, lens)
			
	if debug:
		print word
		for w in stems:
			print " ",w
		print

	return stems


## 문자열 중에 첫 한글 문자의 위치를 반환. 
## return -1 if not found

def findhanchar(str, start=0):
	n = start
	while n < len(str):
		if ord(str[n]) > 128: return n
		n += 1
	return -1


if __name__ == "__main__":
	print LENG	

	stemHanWord("과학적인", 1)
	stemHanWord("우수한", 1)

	stem_complex("위치기반서비스", 1)

	stem_complex("위치기반", 1)

	stem_complex("성추행사건", 1)

