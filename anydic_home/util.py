#!/usr/bin/env python
# -*- coding: EUC-KR -*-

## a, n, A(자)형의, A형 틀(무거운 물건, 호이스트, 샤프트, 파이프 등을 받치는 데 씀)
## --> remove_in_marks(str, '(', ')' )
## a, n, A형의, A형 틀
def remove_in_marks(str, mark1, mark2):
	res = ''
	pos = 0
	while 1:
		p = str[pos:].find(mark1)
		if p == -1: 
			res += str[pos:]
			break

		else:
			res += str[pos:pos+p]
			pos = pos+p+1

			p = str[pos:].find(mark2)
			if p == -1: break
			pos = pos+p+1

	return res


def getinput(prompt):
	try:
		a = raw_input(prompt).strip()
	except: 
		a = '.'

	return a

if __name__ == "__main__":

	pass


