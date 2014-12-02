#include <string.h>
#include <ctype.h>
#include <stdlib.h> // qsort()

#include "StrCmp.h"


int StrCmp::_cmpstr (const void *a, const void *b)
{
	return strcmp ((char *)*(char **)a, (char *)*(char **)b);
}

void StrCmp::sort(char **arr, char size)
{
	qsort(arr, size, sizeof(char *), _cmpstr);
}

/**
'?' �� ���Ե� strcmp(). '?'�� �� ���ڿ� �ش� 
*/
int StrCmp::anyncmp(char *A, char *B, int len)
{
	if (len < 1) return 0;
	while (*A && *B && len--) {
		if (*A != '?' && *A != *B)
			return (*A - *B);
		A++; B++;
	}
	return (0);
}

/**
'?' �� ���Ե� strcmp(). '?'�� �� ���ڿ� �ش� 
*/
int StrCmp::anycmp(char *A, char *B)
{
	while (*A && *B) {
		if (*A != '?' && *A != *B)
			return (*A - *B);
		A++; B++;
	}
	return (*A - *B);
}

/**
'*' �� ���Ե� strcmp(). '*'�� 0�� �̻��� ���ڿ� �ش� 
*/
int StrCmp::wildcmp(char *A, char *B)
{
	int	head, a_len, b_len, tail;
	char *ptr=0;
	int	res;
	ptr = strchr(A, '*');
	a_len = strlen(A);
	b_len = strlen(B);
	
	if (ptr) {
		
		head = (int)ptr - (int)A;
		tail = a_len - head -1;
		res = anyncmp(A, B, head);
		if (res || tail == 0) return res;
		else if (b_len < tail) return 1;
		else {
			res = anyncmp(ptr+1, B+b_len-tail, tail);
			if (res == 0) return 0;
			else	return 1;
		}
	}
	else
		return anycmp(A, B);
}

/**
URL ���ڿ����� anycmp()�� �����ϴ� ���̴�. '?' ��ſ� '$?' ��ȣ�� ����Ѵ�.
*/
int StrCmp::URLanycmp(char *A, char *B)
{
	while (*A && *B ) {
		if (*A=='$' && *(A+1)=='?') {
			A += 2;
			if (*B) B++;
		}
		else if (*A != *B) {
			return (*A - *B);
		}
		else {
			A++; B++;
		}
	}
	return (*A - *B);
}

/**
URL ���ڿ����� anycmp()�� �����ϴ� ���̴�. '?' ��ſ� '$?' ��ȣ�� ����Ѵ�.
*/
int StrCmp::URLanyncmp(char *A, char *B, int len)
{
	if (len < 1) return 0;
	while (*A && *B && len--) {
		if (*A=='$' && *(A+1)=='?') {
			A += 2;
			if (*B) B++;
		}
		else if (*A != *B) {
			return (*A - *B);
		}
		else {
			A++; B++;
		}
	}
	return (0);
}

/**
URL ���ڿ����� wildcmp()�� �����ϴ� ���̴�. '*' ��ſ� '$*' ��ȣ�� ����Ѵ�.
*/
int StrCmp::URLwildcmp(char *A, char *B)
{
	int	head, a_len, b_len, tail;
	char *ptr=0;
	int	res;
	ptr = strstr(A, "$*");
	a_len = strlen(A);
	b_len = strlen(B);
	
	if (ptr) {
		
		head = (int)ptr - (int)A;
		tail = a_len - head -2;
		res = URLanyncmp(A, B, head);
		if (res || tail == 0) return res;
		else if (b_len < tail) return 1;
		else {
			res = URLanyncmp(ptr+2, B+b_len-tail, tail);
			if (res == 0) return 0;
			else	return 1;
		}
	}
	else
		return URLanycmp(A, B);
}

/**
 match �Ǵ� ���� ��� 
 */
int StrCmp::match(char *A, char *B, int lenA, int lenB)
{
	while (*A && *B) {
		if (*A != *B) {
		}
		A++; B++;
	}
	return (*A - *B);
}
