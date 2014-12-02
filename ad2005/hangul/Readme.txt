hancode.c 의 ks_wansung_prn() 를 이용하여 완성형 2350자 출력
--> Wangsung.list 화일

iconv 툴을 화용하여  조합형 변환 값을 구함.

iconv -f euc-kr -t unicode Wangsung.list  > Unicode.list
iconv -f euc-kr -t utf-8 Wangsung.list  > Utf-8.list
iconv -f euc-kr -t johab  Wangsung.list  > Johab.list


johapcode.py Johab.list > Johab.code 
