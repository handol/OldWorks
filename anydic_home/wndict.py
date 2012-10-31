#!/usr/bin/env python
# -*- coding: EUC-KR -*-


TWORD = "proceed"

SAMPLE = """\
     v 1: continue with one's activities; "I know it's hard," he
          continued, "but there is no choice"; "carry on--pretend
          we are not in the room" [syn: {continue}, {go on}, {carry
          on}]
     2: move ahead; travel onward; "We proceeded towards
        Washington"; "She continued in the direction of the
        hills"; can also be used in the temporal sense: "We are
        moving ahead in time now" [syn: {go forward}, {continue}]
     3: follow a procedure or take a course; "We should go farther
        in this matter"; "She went through a lot of trouble"; "go
        about the world in a certain manner"; "Messages must go
        through diplomatic channels" [syn: {go}, {move}]
     4: follow a certain course; "The inauguration went well"; "how
        did your interview go?" [syn: {go}]
     5: continue a certain state, condition, or activity; "Keep on
        working!" "We continued to work into the night"; "Keep
        smiling"; "We went on working until well past midnight"
        [syn: {continue}, {go on}, {go along}, {keep}] [ant: {discontinue}]
"""


def proc_syn_ant(expls, n):
	cnt = 1	
	res = ''
	while n < len(expls):	
		m0 = expls.find('[', n)
		m1 = expls.find(':', n)
		m2 = expls.find(']', m1+1)

		if m0 == -1: return res
		if m1 == -1: return res
		if m2 == -1: return res

		s_a = expls[m0+1:m1]
		#print "== %d" % cnt, s_a

		words = expls[m1+1:m2].split(',')
		#print words

		ws = []
		for w in words:
			ws.append( w.strip()[1:-1] )
		#print "==", ws

		n = m2 + 1
		cnt += 1
		
		res += "<font color='green'><b>%s</b></font>" % s_a
		res += " <font color='blue'> %s </font><br/>" % ', '.join(ws)

	return res

def proc_expl_line(expls):
	res = ''
	
	isitalic = 0
	n = 0
	while n < len(expls):

		pos = expls.find(';', n)
		if pos != -1:
			res += expls[n:pos]
			if isitalic:
				res += " </i>"
			res += "<br/>\n"

			isitalic = 1
			res += " &nbsp; <i>"

			n = pos + 1
		else:
			break

	
	pos = expls.find('[', n)
	if pos != -1:
		#print expls[n:]
		res += expls[n:pos]	
		if isitalic:
			res += "</i><br/>\n"

		res2 = proc_syn_ant(expls, pos)
		res = res2 + res

	else:
		res += expls[n:]
		if isitalic:
			res += "</i>\n"

	return res		
		
			

def get_poomsa(line):
	
	pos = line.find(':')

	poomsa = ''
	num= ''

	if pos != -1 and pos < 15: # found
	## 품사 또는 숫자 있는 라인.
		sp = line[:pos].split()

		try:
			if sp[0].isalpha():
				poomsa = sp[0]
				if len(sp) > 1:
					num = sp[1]
			else:
				num = sp[0]
		except:
			return '','',''			
			pass

		expl = line[pos+1:]
	else:
		expl = line

	#print "POOM", line
	#print "POOM", poomsa, num, expl
	return poomsa, num, expl
	

### 한가지 의미의 라인 수 구하기
def count_lines(start, expl_lines):
	cnt = 0
	while start < len(expl_lines):
		#pos = expl_lines[start].find(':')
		#if pos != -1 and not expl_lines[start][pos-1].isalpha(): # found
		if len(expl_lines[start]) > 5 and not expl_lines[start][5].isspace(): 
			break
		start += 1
		cnt += 1	
	return cnt
		


def proc_oneword(word, expl_lines):
	#print word
	#for l in expl_lines	:
	#	print l

	res = ''
	
	n = 0
	while n < len(expl_lines):
		poomsa, num, expl = get_poomsa(expl_lines[n])
		
		if expl == '':
			print "=== some error"
			print word, expl_lines
			break
	
		## 의미, 예문, SYN, ANT 있는 라인.
		cnt = count_lines(n+1, expl_lines)
		#print "CNT=", cnt

		expl += ' '.join( map(str.strip, expl_lines[n+1:n+1+cnt]) )

		#print expl

		#expl_deco = expl
		expl_deco = proc_expl_line(expl)

		if poomsa =='': poomsa = "&nbsp"

		res += "<table><tr><td valign=top><b>%s</b></td> <td valign=top>%s:</td> <td>%s</td></tr></table>\n" %  (poomsa, num, expl_deco)
		#print res

		n += cnt + 1

	return res



WORD = None
NEXTW = None
EXPL = []

fd = None

def init(dictfile):
	global fd, WORD, NEXTW
	try:
		fd = open(dictfile)
	except:
		print "read fail:", dictfile
		return
	


	## 디비 설명 부분은 스킵.
	while 1:
		line = fd.readline()	
		if not line[0].isspace() and not line.startswith("00"): 
				break

	NEXTW = line.strip()

def next():
	global WORD,NEXTW,EXPL
	WORD = NEXTW
	EXPL = [] 

	while 1:
		line = fd.readline()	
		if len(line) > 0 and line[0].isspace():
			EXPL.append(line)
		else:
			break
	NEXTW = line.strip()
	return WORD,EXPL
		
def proc_wndict(dictfile, out=None):
	cnt = 0
	init(dictfile)

	while 1:
		w,expl = next()
		if w == '': break
		#print w, expl
		html = proc_oneword(w, expl)
		if out != None: 
			out.write(w)
			out.write('\n')
			out.write(html)
			out.write('\n')

		cnt += 1
	
	print "Total:", cnt
 	
if __name__ == "__main__":
	import time
	import sys

	t = time.time()

	out = None
	if len(sys.argv) > 1:
		try:
			out = open(sys.argv[1], "w")
		except:
			print "cannot write:", sys.argv[1]
	
		proc_wndict('/data1/AD/data/wn17.dict', out)

	else:
		proc_oneword(TWORD, SAMPLE.split('\n'))

	print "WordNet %.3f" % (time.time()-t)


