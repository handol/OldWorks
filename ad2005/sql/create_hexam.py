#!/usr/bin/env python
# -!- encoding=euc-kr -!-


"""
기본 자음(14)  ㄱ, ㄴ, ㄷ, ㄹ, ㅁ, ㅂ, ㅅ, ㅇ, ㅈ, ㅊ, ㅋ, ㅌ, ㅍ, ㅎ
기본 모음(10) ㅏ, ㅑ, ㅓ,ㅕ, ㅗ, ㅛ, ㅜ, ㅠ, ㅡ    (X)
쌍자음(5) ㄲ, ㄸ, ㅃ, ㅆ, ㅉ
"""


def delete_ex_table():
  for i in range(21):
    print "DELETE FROM hExam_%d;" % (i+1)

def drop_ex_table():
  for i in range(21):
    print "DROP TABLE hExam_%d;" % (i+1)
    
def create_ex_table(number):
	print "CREATE TABLE hExam_%d (" % (number)
	print """\
	id	int  not null,
	doc_id	int  not null,
	s_pos	int  not null,
	s_len	smallint  not null,
	w_pos	smallint  not null,
	w_len	smallint  not null,
	level tinyint  not null,
	conj int  not null,
	PRIMARY KEY(id, doc_id, s_pos) );
	GO"""
	print


delete_ex_table()
print
print
for i in range(21):
	create_ex_table ( i+1 )


