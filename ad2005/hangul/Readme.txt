hancode.c �� ks_wansung_prn() �� �̿��Ͽ� �ϼ��� 2350�� ���
--> Wangsung.list ȭ��

iconv ���� ȭ���Ͽ�  ������ ��ȯ ���� ����.

iconv -f euc-kr -t unicode Wangsung.list  > Unicode.list
iconv -f euc-kr -t utf-8 Wangsung.list  > Utf-8.list
iconv -f euc-kr -t johab  Wangsung.list  > Johab.list


johapcode.py Johab.list > Johab.code 
