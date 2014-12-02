#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "syllable_info_table.h"
#include "josa_table.h"
#include "eomi_table.h"
#include "chaon_dic.h"
#include "yongon_dic.h"
#include "busa_dic.h"
#include "kwanhyungsa_dic.h"
#include "gamtansa_dic.h"
#include "ma.h"
#include "verb_incomp_dic.h"
#define KSC_START 0xb0a1

int table_lcon_to_fcon[] =
{
	0,
	0, 2, 3, 0, 4,
	0, 0, 5, 7, 0,
	0, 0, 0, 0, 0,
	0, 8, 0, 9, 0,
	11, 12, 0, 14, 16,
	17, 18, 19, 20
};
//extern unsigned char token[100][100];
extern STR_RESULT_MA result_ma[100];

extern int convert_3_to_ks(unsigned int f, unsigned int m, unsigned int l, unsigned char * des);
extern int token_to_johabs(unsigned char *str,int len,unsigned char token_johab[20][3]);

int
check_belong_to_info_table_chaon (unsigned char *str, int len)
{
	unsigned int index;
	unsigned int flag;
	if (len == 1)
		flag = F_CHAON1;
	else if (len == 2)
		flag = F_CHAON2;
	else if (len == 3)
		flag = F_CHAON3;
	else if (len == 4)
		flag = F_CHAON4;
	else
		flag = F_CHAON5;

	index = str[0] * 256 + str[1] - KSC_START;
	if ((table_syllable_info[index] & flag) > 0)
		return 1;
	else if ((table_syllable_info[index] & F_PRONOUN) > 0)
		return 1;
	else
		return 0;
}


int
check_belong_to_info_table_yongon (unsigned char *str, int len)
{
	unsigned int index;
	unsigned int flag;
	//printf(" len of yongon=%d \n",len);
	if (len == 1)
		flag = F_YONGON1;
	else if (len == 2)
		flag = F_YONGON2;
	else
		flag = F_YONGON3;
	index = str[0] * 256 + str[1] - KSC_START;
	if ((table_syllable_info[index] & flag) > 0)
		return 1;
	else
		return 0;
}


int
check_belong_to_info_table_busa (unsigned char *str, int len)
{
	unsigned int index;
	unsigned int flag;
	if (len == 1)
		flag = F_ADV1;
	else if (len == 2)
		flag = F_ADV2;
	else if (len == 3)
		flag = F_ADV3;
	else if (len == 4)
		flag = F_ADV4;
	else
		flag = F_ADV5;
	index = str[0] * 256 + str[1] - KSC_START;
	if ((table_syllable_info[index] & flag) > 0)
		return 1;
	else
		return 0;
}


int
check_belong_to_info_table (unsigned char *str, unsigned int flag)
{
	unsigned int index;
	index = str[0] * 256 + str[1] - KSC_START;
	if ((table_syllable_info[index] & flag) > 0)
		return 1;
	else
		return 0;
}


int
check_connection_cj (unsigned char *chaon, int len, unsigned char *fvl,
unsigned char *str)
{
	/*
	  unsigned char tmp[9];
	  convert_3_to_ks(fvl[0],fvl[1],fvl[2],tmp);
	  printf(" lsat chaon=%s \n",tmp);
	*/
	if (len == 2 &&
		(!strncmp ((char*)chaon, "��", 2) || !strncmp ((char*)chaon, "��", 2)
		|| !strncmp ((char*)chaon, "��", 2)))
	{
		if (!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2))
			return 1;
		else
			return 0;
	}
	else if (!strcmp ((char*)(char*)chaon, "��") || !strncmp ((char*)chaon, "��", 2)
		|| !strcmp ((char*)chaon, "��"))
	{
		if (!strcmp ((char*)str, "��"))
			return 0;
	}
	else if (!strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2))
	{
		if (fvl[2] == 1)
			return 0;		  // ��ħ�� ���� ���� �Ұ���
		else
			return 1;
	}
	else if (!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2) ||
		!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2))
	{
		if ((int) fvl[2] > 1)
			return 0;
		return 1;
	}
	else if (!strncmp ((char*)str, "��", 2))
	{
		if (fvl[2] == 1 || fvl[2] == 9)
			return 1;		  // �����̳� �� �� �����߸�
		else
			return 0;
	}
	return 1;
}


int
check_connection_ye (unsigned char *yongon, int len,
unsigned char *fvl, unsigned char *str)
{
	return 1;
}


int
check_yongon_only (unsigned char *str, int len)
{
	// ��𿡼��� ���Ǵ� ����,
	// ���� �� ������ �ϳ��� ������ ������ ü��Ž���� ���� �ʾƵ� �ȴ�.
	int i;
	for (i = 0; i < len; i += 2)
	{
		if (check_belong_to_info_table (str + i, F_ETC))
			return 1;
	}
	return 0;
}


int
is_belong_to_josa_table (unsigned char *str)
{
	// ������ ���� Ž���� �ؾ������� , ������ ������ ���糪 ��̷� ��ϵ��� ����
	// �ܾ ���Ƽ� �ʿ��� ��� ������  �ܾ �߰��ؾ� �ϱ� ������
	// �ϴ� �������� Ž���� �����ϵ��� �ڵ�
	int i;
	int len;
	len = strlen ((char*)str);
	if (len == 2)
		if (check_belong_to_info_table (str, F_JOSA1))
			return 1;
	else
		return 0;
	for (i = 0; i < NO_JOSA; i++)
		if (!strcmp ((char*)str, table_josa[i]))
			return 1;
	return 0;
}


int
is_belong_to_eomi_table (unsigned char *str)
{
	// ���� �������� ������ �ϴ��� �������� Ž��
	// ���� Ž���� �Ϸ��� �ܾ ������� �迭�Ǿ�� ��
	int i;
	int len;
	len = strlen ((char*)str);
	if (len == 2)
	{
		if (!strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2)
			|| !strncmp ((char*)str, "��", 2) || !strncmp ((char*)str, "��", 2))
			return 1;
		else if (check_belong_to_info_table (str, F_EOMI1))
			return 1;
		else
			return 0;
	}
	for (i = 0; i < NO_EOMI; i++)
		if (!strcmp ((char*)str, table_eomi[i]))
			return 1;
	return 0;
}


// �������� ����Ž���� �ϱ� ���� �Լ�
// ���� ũ���� �ܾ�� ������ ���������� offset �� �ε����� ����ϰ�
// ��Ʈ�� ���� �ε����� ����� ������ �����ϸ�
// strncmp ((char*)strcmp) �� ����Ž���� �� �� �ִ�.
int
b_search (int kind, FILE * fp, long start, long end, unsigned char *str, int len)
{
	char str_read[20];
	long half;
	int val;
	int no_of_word;
	if (start == end - 2 * len)
	{						  // �ѱ��� 2����Ʈ�̹Ƿ�
		fseek (fp, start, SEEK_SET);
		fread (str_read, len, 1, fp);
		/* if(!strncmp(str,"��",2) ) {
		str_read[len]=0;
		printf(" s=%d e=%d str_read=%s \n",start,end,str_read);
		}
		*/
		if (!strncmp ((char*)str, str_read, len))
			return 1;
		fread (str_read, len, 1, fp);
		if (!strncmp ((char*)str, str_read, len))
			return 1;
		return 0;
	}
	else
	{
		half = start + (end - start) / 2;
		if (kind == CHAON)
			no_of_word = no_of_chaon[len / 2 - 1];
		else if (kind == YONGON)
		{
			no_of_word = no_of_yongon[len / 2 - 1];
		}
		else if (kind == BUSA)
		{
			no_of_word = no_of_busa[len / 2 - 1];
		}
		else if (kind == KWANHYUNGSA)
		{
			no_of_word = no_of_kwanhyungsa[len / 2 - 1];
		}
		else if (kind == VERB_INCOMP)
		{
			no_of_word = no_of_verb_incomp[len / 2 - 1];
		}
		else if (kind == GAMTANSA)
		{
			no_of_word = no_of_gamtansa[len / 2 - 1];
		}
		if (half > no_of_word * len)
		{
			return b_search (kind, fp, start, half, str, len);
		}
		else
		{
			fseek (fp, half, SEEK_SET);
			fread (str_read, len, 1, fp);
			//if(!strncmp(str,"��",2)) printf("str_read=%s \n",str_read);
			val = strncmp ((char*)str, str_read, len);
			if (val == 0)
				return 1;
			else if (val < 0)
				return b_search (kind, fp, start, half, str, len);
			else
				return b_search (kind, fp, half, end, str, len);
		}
	}
}


int
is_belong_to_chaon_dic (unsigned char *str)
{
	FILE *fp;
	int len;
	unsigned char *p_str;
	char fname[20];
	int result;
	len = strlen ((char*)str);
	if (len == 2)
		if (check_belong_to_info_table (str, F_CHAON1))
			return 1;
	if (!check_belong_to_info_table_chaon (str + len - 2, len / 2))
		return 0;
	//printf("str=%s \n",str);
	// �� ������ ü���� �������� �� �� �ִ��� üũ
	// ����Ž�� Ƚ���� ���̱� ����
	if (len / 2 > 10)
		return 1;			  //�ϴ� �̺���  �� ���ڴ� ������ ü������
	sprintf (fname, "chaon_%d.dic", len / 2);
	fp = fopen (fname, "r");

	result =
		b_search (CHAON, fp, 0, initial_end_chaon[len / 2 - 1] * len, str, len);
	if (result == 0)
	{
		p_str = str + len - 2;
		if (!strncmp ((char*)p_str, "��", 2) || !strncmp ((char*)p_str, "��", 2) ||
			!strncmp ((char*)p_str, "��", 2) || !strncmp ((char*)p_str, "��", 2))
		{
			*p_str = 0;
			result =
				b_search (CHAON, fp, 0, initial_end_chaon[len / 2 - 1] * len, str,
				len);
		}
	}
	fclose (fp);
	return result;
}


int
is_belong_to_yongon_dic (unsigned char *str)
{
	FILE *fp;
	int len;
	char fname[20];
	int result;
	len = strlen ((char*)str);
	if (!strncmp ((char*)str, "��", 2))
		printf ("str =%s \n");
	if (len == 2)
		if (check_belong_to_info_table (str, F_YONGON1))
			return 1;
	else
		return 0;
	if (!check_belong_to_info_table_yongon (str + len - 2, len / 2))
		return 0;
	// ü���� ���� ��������. ���� ü���� ������ ��������
	// �ȣ��ȸ���� �����Ƿ� �ش��Լ��� ȣ���ϱ� �ٷ����� üũ�ص� ����

	if (len / 2 > 6)
		return 0;			  // �� ���ڴ� ��𿡼� ����
	sprintf (fname, "yongon_%d.dic", len / 2);
	fp = fopen (fname, "r");
	result =
		b_search (YONGON, fp, 0, initial_end_yongon[len / 2 - 1] * len, str, len);
	fclose (fp);
	return result;
}


int
is_belong_to_busa_dic (unsigned char *str)
{
	FILE *fp;
	int len;
	char fname[20];
	int result;
	len = strlen ((char*)str);
	if (len == 2)
		if (check_belong_to_info_table (str, F_ADV1))
			return 1;
	else
		return 0;
	if (len / 2 > 7)
		return 0;			  // �� ���ڴ�  ����
	sprintf (fname, "busa_%d.dic", len / 2);
	fp = fopen (fname, "r");
	result =
		b_search (BUSA, fp, 0, initial_end_busa[len / 2 - 1] * len, str, len);
	fclose (fp);
	return result;
}


int
is_belong_to_kwanhyungsa_dic (unsigned char *str)
{
	FILE *fp;
	int len;
	char fname[20];
	int result;
	len = strlen ((char*)str);
	if (len / 2 > 4)
		return 0;			  // �� ���ڴ�  ����
	sprintf (fname, "kwanhyungsa_%d.dic", len / 2);
	fp = fopen (fname, "r");
	result = b_search (KWANHYUNGSA, fp, 0,
		initial_end_kwanhyungsa[len / 2 - 1] * len, str, len);
	fclose (fp);
	return result;
}


int
is_belong_to_gamtansa_dic (unsigned char *str)
{
	FILE *fp;
	int len;
	char fname[20];
	int result;
	len = strlen ((char*)str);
	if (len / 2 > 5)
		return 0;			  // �� ���ڴ�  ����
	sprintf (fname, "gamtansa_%d.dic", len / 2);
	fp = fopen (fname, "r");
	result =
		b_search (GAMTANSA, fp, 0, initial_end_gamtansa[len / 2 - 1] * len, str,
		len);
	fclose (fp);
	return result;
}


// ü��+ ������������ ��� ������ ���簡 ��̺�ȭ�� ���� ��ȭ�� ��
int
is_chaon_plus_yi (unsigned char *str_org)
{
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	if (len <= 2)
		return 0;
	if (strncmp ((char*)str + len - 2, "��", 2))
		return 0;
	str[len - 2] = 0;
	return is_belong_to_chaon_dic (str);
}


// ü��+�ϴٷ� �� ����� ������ ��ϵ��� ���� ��찡 ����
// �׷��� �̰�쵵 �����̹Ƿ� �����ؾ� �Ѵ�.
// ü��+������ � �����ؾ� ������ �ϴ��� ����
int
is_dongsa_with_ha (unsigned char *str_org)
{
	// ���� Ȳ���� ����� �ش�.
	// ���� ��� ���ؾ� ��� ����
	//  ���� ��� �� �������� �� �ִµ�,
	// ���ϴ� �ٽ� ��+�� �� ���������� �� �� ü���� �� �� �����Ƿ�
	// ��ġ ���ϴ� ��� ����� �ؼ�
	// �̷� ���� �������� �ϴٿ� �����ؼ� ���簡 �� �� �ִ�����
	// ǥ���ؾ� �ϴµ� �׷��� ���� �ʹ� ����..
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	if (strncmp ((char*)str + len - 2, "��", 2))
		return 0;
	if (len == 2)
		return 1;			  // �� ����
	str[len - 2] = 0;
							  // ü��+�� ����
	return is_belong_to_chaon_dic (str);
}


// ���̴� �� ���� ��� �� �� ������ ����
// �̷��� �����  ������ ���ս� ��+��-> �� ���� �Ǳ� ������
// �� �Լ��� ������ Ž���� �ʿ�
int
is_dongsa_with_yi (unsigned char *str_org)
{
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	strncpy ((char*)str + len, (char*) "��", 2);
	str[len + 2] = 0;
	return is_belong_to_yongon_dic (str);
}


//��� ��� �и�
int
treat_pf (unsigned char *str, unsigned char *str_first,
unsigned char *str_middle,
unsigned char *fvl_tmp, int ipos, int len)
{
	unsigned char tmp[9];

	if (!strncmp ((char*)str + ipos, "��", 2) || !strncmp ((char*)str + ipos, "��", 2))
	{						  // ��, ��

		if (ipos > 2 && !strncmp ((char*)str + ipos - 2, "��", 2))
		{					  // ��� �������̻�
			// ��� ��� �ô� �������� ������ ���ܿ��� ������ �� �̳� �� �� �´�.
			if (ipos > 4 && !strncmp ((char*)str + ipos - 4, "��", 2))
			{				  // �� �տ� �� �� ������
				strncpy ((char*)str_middle, (char*) str + ipos - 4, len - ipos + 4);
				str_middle[len - ipos + 4] = 0;
				strncpy ((char*)str_first, (char*) str, ipos - 4);
				str_first[ipos - 4] = 0;
				return ipos - 4;
			}
			else
			{
				strncpy ((char*)str_middle, (char*) str + ipos - 2, len - ipos + 2);
				str_middle[len - ipos + 2] = 0;
				strncpy ((char*)str_first, (char*) str, ipos - 2);
				str_first[ipos - 2] = 0;
				return ipos - 2;
			}
		}
		else
		{					  // '��'�� '����'�� �ƴ� ��� ipos ���� ��� ���
			strncpy ((char*)str_middle, (char*) str + ipos, len - ipos);
			str_middle[len - ipos] = 0;
			strncpy ((char*)str_first, (char*) str, ipos);
			str_first[ipos] = 0;
			return ipos;
		}
		return ipos;
	}
	else if (!strncmp ((char*)str + ipos, "��", 2) || !strncmp ((char*)str + ipos, "��", 2))
	{						  //'��", '��' �տ��� �ð� ���� �� ��
		strncpy ((char*)str_middle, (char*) str + ipos, len - ipos);
		str_middle[len - ipos] = 0;
		strncpy ((char*)str_first, (char*) str, ipos);
		str_first[ipos] = 0;
		return ipos;
	}
	else if (!strncmp ((char*)str + ipos, "��", 2))
	{						  // '��' �տ��� '��' �� �� �� ����
		// �ϴ� ����о�̷� ������ �� �Ŀ� �νôٿ� ���� ��� �� ��� ��̰� �÷�
		// ���۵Ǵ� ��� �����

		if (ipos > 2 && !strncmp ((char*)str + ipos - 2, "��", 2))
		{					  //�տ� �� �� ������
			strncpy ((char*)str_middle, (char*) str + ipos - 2, len - ipos + 2);
			str_middle[len - ipos + 2] = 0;
			strncpy ((char*)str_first, (char*) str, ipos - 2);
			str_first[ipos - 2] = 0;
			return ipos - 4;
		}
		else
		{
			strncpy ((char*)str_middle, (char*) "�þ�", 4);
			strncpy ((char*)str_middle + 4, (char*) str + ipos + 2, len - ipos - 2);
			str_middle[len - ipos + 2] = 0;
			strncpy ((char*)str_first, (char*) str, ipos);
			str_first[ipos] = 0;
			return ipos;
		}
	}
	else if (!strncmp ((char*)str + ipos, "��", 2))
	{
		strncpy ((char*)str_middle, (char*) "��", 2);
		strncpy ((char*)str_middle + 2, (char*) str + ipos + 2, len - ipos - 2);
		str_middle[len - ipos] = 0;
		strncpy ((char*)str_first, (char*) str, ipos);
		strncpy ((char*)str_first + ipos, (char*) "��", 2);
		str_first[ipos + 2] = 0;
		return ipos + 2;
	}
							  // ��, ��
	else if (fvl_tmp[1] == 3 || fvl_tmp[1] == 4 ||
		fvl_tmp[1] == 7 ||	  // ��,
							  // ��, ��
		fvl_tmp[1] == 14 || fvl_tmp[1] == 15 ||
		fvl_tmp[1] == 11 || fvl_tmp[1] == 21)
	{						  // ��, ��,
		convert_3_to_ks (fvl_tmp[0], fvl_tmp[1], 1, tmp);
		strncpy ((char*)str_first, (char*) str, ipos);
		strncpy ((char*)str_first + ipos, (char*) tmp, 2);
		str_first[ipos + 2] = 0;
		strncpy ((char*)str_middle, (char*) "��", 2);
		strncpy ((char*)str_middle + 2, (char*) str + ipos + 2, len - ipos - 2);
		str_middle[len - ipos] = 0;
		return ipos;
	}
}


void
print_result_chaon (unsigned char *str_first, unsigned char *str_last)
{
	printf ("(%s/ü��)+(%s/����) \n", str_first, str_last);
}


void
print_result_yongon (unsigned char *str_first,
unsigned char *str_middle, unsigned char *str_last)
{

	printf ("(%s/���)", str_first);
	if (str_middle)
		printf ("+(%s/������)", str_middle);
	printf ("+(%s/���) \n", str_last);
}


void
print_result_yongon_plus_yi (unsigned char *str_first,
unsigned char *str_middle,
unsigned char *str_last)
{
	printf ("%s/(ü��+����������)", str_first);
	if (str_middle)
		printf ("+(%s/������)", str_middle);
	printf ("+(%s/���) \n", str_last);
}


void
print_result_word (int nth_word)
{
	int type;
	L_RESULT_MA *l_result_ma;

	l_result_ma = result_ma[nth_word].p_list_result;
	if (l_result_ma == NULL)
		return;
	while (l_result_ma != NULL)
	{
		type = l_result_ma->type;

		if (type == EOGAN_PEOMI_EOMI)
		{
			print_result_yongon (l_result_ma->yongon.eogan,
				l_result_ma->yongon.peomi,
				l_result_ma->yongon.eomi);
		}
		else if (type == EOGAN_EOMI)
		{
			print_result_yongon (l_result_ma->yongon.eogan, NULL,
				l_result_ma->yongon.eomi);
		}
		else if (type == CHAON_YI_PEOMI_EOMI)
		{
			print_result_yongon_plus_yi (l_result_ma->yongon.eogan,
				l_result_ma->yongon.peomi,
				l_result_ma->yongon.eomi);
		}
		else if (type == CHAON_YI_EOMI)
		{
			print_result_yongon_plus_yi (l_result_ma->yongon.eogan,
				NULL, l_result_ma->yongon.eomi);

		}
		else if (type == CHAON_JOSA)
		{
			print_result_chaon (l_result_ma->chaon.chaon,
				l_result_ma->chaon.josa);
		}
		else if (type == S_BUSA)
		{
			printf ("(%s/�λ�) \n", l_result_ma->single);
		}
		else if (type == S_KWANHYUNGSA)
		{
			printf ("(%s/������) \n", l_result_ma->single);
		}
		else if (type == S_CHAON)
		{
			printf ("(%s/ü��) \n", l_result_ma->single);
		}
		else if (type == S_GAMTANSA)
		{
			printf ("(%s/��ź��) \n", l_result_ma->single);
		}
		l_result_ma = l_result_ma->next;
	}
}


void
print_result (int no_of_word)
{
	int i;
	for (i = 0; i < no_of_word; i++)
		print_result_word (i);
}


void
free_list_result (int no_of_word)
{
	L_RESULT_MA *p_list_result;
	L_RESULT_MA *p_list_tmp;
	int i;
	for (i = 0; i < no_of_word; i++)
	{
		p_list_result = result_ma[i].p_list_result;
		while (p_list_result != NULL)
		{
			p_list_tmp = p_list_result;
			p_list_result = p_list_result->next;
			free (p_list_tmp);
		}
	}
}


void
init_list_result (int no_of_word)
{
	int i;
	for (i = 0; i < no_of_word; i++)
	{
		result_ma[i].no_of_result = 0;
		result_ma[i].p_list_result = NULL;
	}
}


void
save_result_single_word (int nth_word, unsigned char *str, int type)
{
	L_RESULT_MA *p_list_result;
	p_list_result = (L_RESULT_MA*)calloc (sizeof (L_RESULT_MA), 1);
	strcpy ((char*)p_list_result->single, (char*) str);
	p_list_result->type = type;
	if (result_ma[nth_word].no_of_result == 0)
	{
		result_ma[nth_word].p_list_result = p_list_result;
		result_ma[nth_word].p_list_result->next = NULL;
	}
	else
	{
		result_ma[nth_word].p_list_result->prev = p_list_result;
		p_list_result->next = result_ma[nth_word].p_list_result;
		result_ma[nth_word].p_list_result = p_list_result;
	}
	result_ma[nth_word].no_of_result++;
	print_result_word (nth_word);
}


void
save_result_chaon (int nth_word, unsigned char *str_first,
unsigned char *str_last)
{
	L_RESULT_MA *p_list_result;
	p_list_result = (L_RESULT_MA*)calloc (sizeof (L_RESULT_MA), 1);
	p_list_result->type = CHAON_JOSA;
	strcpy ((char*)p_list_result->chaon.chaon, (char*) str_first);
	strcpy ((char*)p_list_result->chaon.josa, (char*) str_last);
	if (result_ma[nth_word].no_of_result == 0)
	{
		result_ma[nth_word].p_list_result = p_list_result;
		result_ma[nth_word].p_list_result->next = NULL;
	}
	else
	{
		result_ma[nth_word].p_list_result->prev = p_list_result;
		p_list_result->next = result_ma[nth_word].p_list_result;
		result_ma[nth_word].p_list_result = p_list_result;
	}
	result_ma[nth_word].no_of_result++;
	//  print_result_word(nth_word);
}


void
save_result_yongon (int nth_word, unsigned char *str_first,
unsigned char *str_middle, unsigned char *str_last)
{
	L_RESULT_MA *p_list_result;
	p_list_result = (L_RESULT_MA*)calloc (sizeof (L_RESULT_MA), 1);
	strcpy ((char*)p_list_result->yongon.eogan, (char*) str_first);
	if (str_middle)
	{
		strcpy ((char*)p_list_result->yongon.peomi, (char*) str_middle);
		p_list_result->type = EOGAN_PEOMI_EOMI;
	}
	else
		p_list_result->type = EOGAN_EOMI;
	strcpy ((char*)p_list_result->yongon.eomi, (char*) str_last);
	if (result_ma[nth_word].no_of_result == 0)
	{
		result_ma[nth_word].p_list_result = p_list_result;
		result_ma[nth_word].p_list_result->next = NULL;
	}
	else
	{
		result_ma[nth_word].p_list_result->prev = p_list_result;
		p_list_result->next = result_ma[nth_word].p_list_result;
		result_ma[nth_word].p_list_result = p_list_result;
	}
	result_ma[nth_word].no_of_result++;
	//  print_result_word(nth_word);
}


void
save_result_yongon_plus_yi (int nth_word, unsigned char *str_first,
unsigned char *str_middle,
unsigned char *str_last)
{
	L_RESULT_MA *p_list_result;
	p_list_result = (L_RESULT_MA*)calloc (sizeof (L_RESULT_MA), 1);
	strcpy ((char*)p_list_result->yongon.eogan, (char*) str_first);
	if (str_middle)
	{
		strcpy ((char*)p_list_result->yongon.peomi, (char*) str_middle);
		p_list_result->type = CHAON_YI_PEOMI_EOMI;
	}
	else
		p_list_result->type = CHAON_YI_EOMI;
	strcpy ((char*)p_list_result->yongon.eomi, (char*) str_last);
	if (result_ma[nth_word].no_of_result == 0)
	{
		result_ma[nth_word].p_list_result = p_list_result;
		result_ma[nth_word].p_list_result->next = NULL;
	}
	else
	{
		result_ma[nth_word].p_list_result->prev = p_list_result;
		p_list_result->next = result_ma[nth_word].p_list_result;
		result_ma[nth_word].p_list_result = p_list_result;
	}
	result_ma[nth_word].no_of_result++;
	//  print_result_word(nth_word);
}


int
rule_regular (unsigned char *str_first,
unsigned char *str_middle, unsigned char *str_last, int nth)
{
	int status;
	status = 0;

							  // ���̴�, ���̴ٿ� ���� 2������ ������ ��
	if (is_chaon_plus_yi (str_first))
		save_result_yongon_plus_yi (nth, str_first, str_middle, str_last);
							  // else �� ������� ����
	if (is_dongsa_with_ha (str_first) ||
		is_belong_to_yongon_dic (str_first))
		save_result_yongon (nth, str_first, str_middle, str_last);

}


int
rule_irregular1 (unsigned char *str_first,
unsigned char *str_last,
unsigned char *fvl_prev, unsigned char *fvl, int nth)
{
	int len;
	unsigned char *syllable_last;
	unsigned char tmp[9];

	len = strlen ((char*)str_first);
	syllable_last = str_first + len - 2;

	//printf(" len=%d str=%s last=%s fvl_pre[2]=%d fvl[2]=%d \n",
	//          len,str_first,syllable_last,fvl_prev[2],fvl[0]);
	if (fvl_prev[2] == 9 && fvl[0] == 13)
	{						  // ������ �� , �ʼ��� ��
		// ��� ����(�׷��� ��̴� ����)
		// �� �ұ�Ģ : �ɾ�= ��+��  �� �� -> �� �� �ٲ�� ��� p.61
		if (check_belong_to_info_table (syllable_last, F_BUL_D))
		{
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 8, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
			{
				save_result_yongon (nth, str_first, NULL, str_last);
			}
		}
	}
	else if (fvl_prev[2] == 1 && fvl[0] == 13)
	{
		//              ( !strncmp(str+i,"��",2) || !strncmp(str+i,"��",2) ) ){
		// �� �ұ�Ģ : ����= �� + �� p.62
		if (check_belong_to_info_table (syllable_last, F_BUL_S))
		{
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 21, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
	}
	else if (fvl_prev[2] == 1 &&
		(fvl[0] == 4 || fvl[0] == 7 || fvl[0] == 8 || fvl[0] == 9))
	{
		// �� �ұ�Ģ : �Ķ�ϱ� = �Ķ�+�ϱ�   �� �� ��,��,��,�� �տ��� Ż��?
		if (check_belong_to_info_table (syllable_last, F_BUL_H))
		{
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 29, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
		if (fvl[0] == 4
			&& check_belong_to_info_table (syllable_last, F_BUL_DEL_L))
		{
			// �� Ż�� : ����=��+��
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 9, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
	}
}


// ��� ��̰� �ִ� ��� ���� ���� ��� ���̰� �ִ�.
// ���� ��� �����̰� �ִ� ��� ���ұ�Ģ�̳� �� Ż���� �� �� ����.
// �� �� ���Ծ��� �� ���� �� �ұ�Ģ�� ���� ��̰� �̹� ������ �����̴�.
// ��� ��̰� ���� ���ͼ� ���� ���� ��̰� '�Ƽ�' �̹Ƿ� '��' �� Ž���ϴ�
// ���������� ��̸� ������ �� ����.
// ���� �ұ�Ģ�� �� �ұ�Ģ�� ���Խ��ѵ� �ȴ�.
int
rule_irregular_with_pf (unsigned char *str_first, unsigned char *str_middle,
unsigned char *str_last,
unsigned char *fvl_prev, unsigned char *fvl, int nth)
{
	int len;
	unsigned char *syllable_last;
	unsigned char str_tmp[20];
	unsigned char tmp[9];

	len = strlen ((char*)str_first);
	syllable_last = str_first + len - 2;
	//printf(" len=%d str=%s last=%s fvl_pre[2]=%d fvl[1]=%d ,fvl[2]=%d \n",
	//          len,str_first,syllable_last,fvl_prev[2],fvl[1],fvl[2]);
	if (!strncmp ((char*)str_middle, "��", 2))
	{
		// '��'+'��'
		if (is_dongsa_with_ha (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);
		// '��' = '��'+'��'
		if (is_dongsa_with_yi (str_first))
		{
			strncpy ((char*)str_first + len, (char*) "��", 2);
			str_first[len + 2] = 0;
			strncpy ((char*)str_middle, (char*) "��", 2);
			save_result_yongon (nth, str_first, str_middle, str_last);
		}
	}
	else if (fvl_prev[2] == 9 && fvl[0] == 13)
	{						  // ������ �� , �ʼ��� ��
		// ��� ����(�׷��� ��̴� ����)
		// �� �ұ�Ģ : �ɾ�= ��+��  �� �� -> �� �� �ٲ�� ��� p.61
		if (check_belong_to_info_table (syllable_last, F_BUL_D))
		{
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 8, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
			{
				save_result_yongon (nth, str_first, str_middle, str_last);
			}
		}
	}
	else if ((fvl_prev[2] == 1 || len <= 2) && fvl[0] == 13)
	{
		//              ( !strncmp(str+i,"��",2) || !strncmp(str+i,"��",2) ) ){
		// �� �ұ�Ģ : ����= �� + �� p.62
		if (check_belong_to_info_table (syllable_last, F_BUL_S))
		{
							  // ��ħ�� �� ����
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 21, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, str_middle, str_last);
		}
							  // ���������� ��/�� �� ���
		if ((fvl[1] == 14 || fvl[1] == 21)
			/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
		{					  // �� �ұ�Ģ
			if (len >= 4)
			{
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				strcpy ((char*)str_tmp, (char*) str_first);
				strncpy ((char*)str_first + len - 4, (char*) tmp, 2);
				str_first[len - 2] = 0;
				//printf("�� �ұ�Ģ %s \n", str_first);
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, str_middle, str_last);
			}
			// ���� -> �� ó�� ����� ���
			if (fvl[1] == 14)  // ��
							  // ��
				convert_3_to_ks (fvl[0], 13, 1, tmp);
			else
							  // ��
				convert_3_to_ks (fvl[0], 20, 1, tmp);
			strncpy ((char*)str_tmp + len - 2, (char*) tmp, 2);
			str_tmp[len];
			if (is_belong_to_yongon_dic (str_tmp))
				save_result_yongon (nth, str_tmp, str_middle, str_last);

		}
		// ��� ��̰� �ִ� ��� �� �̳� �� �� Ż���Ǵ� ���� ����.
		// �׷��� �Ķ��� -> �ķ�����. �� ���� �,��̰� ���ÿ� ���ϴ� ���� ����
	}
	else if (fvl_prev[1] == 4 && fvl_prev[2] == 22 && len >= 4)
	{
		// �� �� ������ �ϰ� ������̷� �� �� �;���
		// �� �ұ�Ģ : �
		if (check_belong_to_info_table (syllable_last, F_BUL_H))
		{
			//        convert_3_to_ks(fvl_prev[0],fvl_prev[1],29,tmp); // ��ħ�� �� ����
			// �̰� � ������ ����
			// �뷨�� ���׷��� ������ ������. �������� �����ϱ� ������ ������
			//        strncpy(str_first+len-2, (char*)tmp,2);
			//        if( is_belong_to_yongon_dic( str_first) )
			save_result_yongon (nth, str_first, str_middle, str_last);
		}
	}
	else if (fvl[1] == 11)
	{						  // �� + ��  -> �� + ��
							  // ��
		convert_3_to_ks (fvl[0], 29, 1, tmp);
		strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
		str_first[len] = 0;
		strncpy ((char*)str_middle, (char*) "��", 2);
		if (is_belong_to_yongon_dic (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);

	}
	else if (fvl[1] == 7)
	{						  // �� ������ ��� �� Ż�� ����
		if (!strncmp ((char*)syllable_last, "��", 2))
		{					  // '��' �ұ�Ģ�� '��' �ұ�Ģ
			if (fvl_prev[2] == 9)
			{
				strcpy ((char*)str_tmp, (char*) str_first);
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
				strncpy ((char*)str_first + len - 4, (char*) tmp, 2);
				strncpy ((char*)str_first + len - 2, (char*) "��", 2);
				if (is_belong_to_yongon_dic (str_first))
				{
					strncpy ((char*)str_middle, (char*) "��", 2);
					save_result_yongon (nth, str_first, str_middle, str_last);
				}
				strncpy ((char*)str_first, (char*) str_tmp, len);
			}
			else if (!strncmp ((char*)str_first + len - 4, "��", 2))
			{
				str_first[len - 2] = 0;
				if (is_belong_to_yongon_dic (str_first))
				{
					strncpy ((char*)str_middle, (char*) "��", 2);
					save_result_yongon (nth, str_first, str_middle, str_last);
				}
			}
		}
		if (!strncmp ((char*)str_first, "��", 2))
							  // p.67
			strncpy ((char*)str_first, (char*) "Ǫ", 2);
		else
		{
			convert_3_to_ks (fvl[0], 27, 1, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			strncpy ((char*)str_middle, (char*) "��", 2);
		}
		if (is_belong_to_yongon_dic (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);

	}
	else if (fvl[1] == 15)
	{						  // �� +�� = �� + ��
		if (strncmp ((char*)syllable_last, "��", 2) && strncmp ((char*)syllable_last, "��", 2)
			&& strncmp ((char*)syllable_last, "��", 2))
		{					  // p.67
			convert_3_to_ks (fvl[0], 18, 1, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, str_middle, str_last);
		}
	}
}


void
treat_eogan (unsigned char *str_first,
unsigned char *str_last,
unsigned char *fvl_prev, unsigned char *fvl, int nth)
{
	//printf(" treat_eogan: \n");
	rule_regular (str_first, NULL, str_last, nth);
	rule_irregular1 (str_first, str_last, fvl_prev, fvl, nth);
}


void
treat_eogan_with_pf (unsigned char *str_first, unsigned char *str_middle,
unsigned char *str_last,
unsigned char *fvl_prev, unsigned char *fvl, int nth)
{
	if (strncmp ((char*)str_middle, "��", 2))
		rule_regular (str_first, str_middle, str_last, nth);
	rule_irregular_with_pf (str_first, str_middle, str_last, fvl_prev, fvl,
		nth);
}


// ����� �������� �Ǵ� ���
int
is_yongon_converted_to_noun (unsigned char *str_org,
unsigned char *fvl_prev,
unsigned char *fvl_pre_prev)
{
	int len;
	unsigned char tmp[9];
	unsigned char str[50];
	unsigned char *p_str;

	len = strlen ((char*)str_org);
	strncpy ((char*)str, (char*) str_org, len);
	str[len] = 0;
	p_str = str;

	if (!strncmp ((char*)p_str + len - 2, "��", 2) || fvl_prev[2] != 17)
		return 0;			  // �� ��ħ
	if (len >= 4)
	{
		if (!strncmp ((char*)p_str + len - 2, "��", 2)
			|| !strncmp ((char*)p_str + len - 2, "��", 2))
		{
			p_str[len - 2] = 0;
			if (is_belong_to_yongon_dic (p_str))
				return 1;
			else
				return 0;
		}
		else if (!strncmp ((char*)p_str + len - 2, "��", 2))
		{					  // ġ���-> ġ�� , ���� -> ����
			strncpy ((char*)p_str + len - 2, (char*) "��", 2);
			if (is_belong_to_yongon_dic (p_str))
				return 1;
			else if (fvl_pre_prev[2] == 1)
			{
							  // �� �߰�
				convert_3_to_ks (fvl_pre_prev[0], fvl_pre_prev[1], 19, tmp);
				strncpy ((char*)p_str + len - 4, (char*) tmp, 2);
				str[len - 2] = 0;
				if (is_belong_to_yongon_dic (p_str))
					return 1;
				else
					return 0;
			}
			else
				return 0;
		}
		else
		{
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
			if (!strncmp ((char*)tmp, "��", 2) || !strncmp ((char*)tmp, "��", 2))
			{
				str[len - 2] = 0;
				if (is_belong_to_chaon_dic (p_str))
					return 1;
				else
					return 0;
			}
			else
			{
				strncpy ((char*)p_str + len - 2, (char*) tmp, 2);
				if (is_belong_to_yongon_dic (p_str))
					return 1;
				else
					return 0;
			}
		}
	}
	else
	{
		convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
		strncpy ((char*)p_str, (char*) tmp, 2);
		if (!strncmp ((char*)tmp, "��", 2) || !strncmp ((char*)tmp, "��", 2))
			return 0;
		if (is_belong_to_yongon_dic (p_str))
			return 1;
		else
			return 0;
	}
}


void
token_to_ma (unsigned char *str, int nth, int position)
{
	char *p_tmp;
	int len;
	int josa_flag;
	int eomi_flag;
	int i;
	int j;
	int ipos;
	int pos_pf;
	int len_word;
	int len_total;
	unsigned char str_first[20];
	unsigned char str_middle[20];
	unsigned char str_last[20];
	unsigned char token_johab[20][3];
	unsigned char *fvl_prev;
	unsigned char *fvl;
	unsigned char *fvl_tmp;
	unsigned char tmp[9];
	unsigned char lcon;
	int status_del;
	int status_yongon_only;

	if (position == 2)
	{
		p_tmp = NULL;
		if ((p_tmp = strchr ((char*)str, '.')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, '?')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, '!')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, ',')) != NULL)
		{
		}

		if (p_tmp != NULL)
		{
			*p_tmp = 0;
			len = strlen ((char*)str);
		}
		else
			len = strlen ((char*)str);

	}
	else
	{
		len = strlen ((char*)str);
	}
	if (len == 0)
		return;
	//printf(" ���Ͼ� �м� ���� \n");
	// ���� ���Ͼ� �� �λ����� üũ
	if (check_belong_to_info_table_busa (str + len - 2, len / 2))
	{
		if (is_belong_to_busa_dic (str))
			save_result_single_word (nth, str, S_BUSA);
		else
		{
			if (!strncmp ((char*)str + len - 2, "��", 2))
			{
				strncpy ((char*)str_first, (char*) str, len - 2);
				str_first[len - 2] = 0;
				if (is_belong_to_yongon_dic (str_first))
					save_result_single_word (nth, str, S_BUSA);
			}
		}
	}
	if (check_belong_to_info_table (str + len - 2, F_DTXL))
	{
		if (is_belong_to_kwanhyungsa_dic (str))
			save_result_single_word (nth, str, S_KWANHYUNGSA);
		if (nth == 0 && is_belong_to_gamtansa_dic (str))
			save_result_single_word (nth, str, S_GAMTANSA);
	}
	if (is_belong_to_chaon_dic (str))
		save_result_single_word (nth, str, S_CHAON);

	//printf(" ���Ͼ�м� ���� \n");

	// ���� ������ ��𿡸� �� �� �ִ� ������ ����ִ��� üũ �ؼ�
	// ��𿡸� �ü� ������ ü��+������ ���ɼ��� ����
	status_yongon_only = check_yongon_only (str, len);
	if (status_yongon_only)
		josa_flag = 0;
	else
		josa_flag = 1;
	eomi_flag = 1;

	len_total = token_to_johabs (str, len, token_johab);

	// ù������ ��̺κ��� ���ԵǾ� ���� ���ɼ��� �ִ°��� �׷��� ���� ��챸��
	// ��1) �� ��� -> �� +��, ���ٰ� -> �� + ���ٰ�
	// ��,�� �� ��������� ù���� ����� ��ħ�� �����Ƿ�
	// ù������ ���� �ȴ�.
	// ��2) �� ���ȴ�. ->�� + �� ����� ������ ��ħ���� ��, ��, ��, �� �� ������
	// ��� ��� '��' �� �� �� �����ǹǷ� ù������ ��̰� �ִٰ� ������ �� �ִ�.
	// ��3) �� = �� + �� �� �������
	// ��4) �� = �� + ��, �� = �� + �� , �� = �� + �� ���
	for (i = len - 2; i >= 0; i -= 2)
	{
		len_word = i / 2;
		fvl = &token_johab[i / 2][0];
		if (i >= 2)
		{
			fvl_prev = &token_johab[i / 2 - 1][0];
			if (josa_flag && check_belong_to_info_table (str + i, F_JOSA1))
			{
				strncpy ((char*)str_first, (char*) str, i);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				if (is_belong_to_josa_table (str_last) &&
					check_connection_cj (str_first, i, fvl_prev, str_last))
					if (is_belong_to_chaon_dic (str_first))
						save_result_chaon (nth, str_first, str_last);
				else
				{
					if (i == 2)
					{
						if (is_yongon_converted_to_noun
							(str_first, fvl_prev, NULL))
							save_result_chaon (nth, str_first, str_last);
					}
					else
					{
						if (is_yongon_converted_to_noun (str_first, fvl_prev,
							token_johab[i / 2 -
							2]))
							save_result_chaon (nth, str_first, str_last);
					}
				}
			}
		}
		// ���⼭���� ���и�����
		// ���� ù�������� üũ�� �ʿ䰡 ���� ���
		if (i >= 2 && eomi_flag)
		{
			// ���� ��̰� ������ �ʴ� ���
			if (check_belong_to_info_table (str + i, F_EOMI1))
			{
				// ���������� ����� ù��° �ü��ִ���
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				//printf(" str_last=%s \n",str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					// ������������ ���ڿ��� ��̻���(���̺�)�� ��ϵǾ� �ִ���
					if (fvl_prev[2] == 22)
					{		  //��������  �� ��ħ�� �ִ� ���
						j = 0;
						while (i / 2 - 2 - j > 0
							&& token_johab[i / 2 - 2 - j][2] == 22)
						{
							j++;
						}
							  // ipos �� �� �� �� ������ ������
						ipos = i - 2 - 2 * j;
						if (strncmp ((char*)str + ipos, "��", 2))
						{	  // �� �� ��� ��̰� �ƴ�
							  // ������ �и�
							pos_pf = treat_pf (str, str_first, str_middle, token_johab[ipos / 2], ipos, i);
							//            fvl_tmp=token_johab[ipos/2];
							//printf(" %s %s %s %d \n",str_first,str_middle,str_last, pos_pf);
							treat_eogan_with_pf (str_first, str_middle,
								str_last,
								token_johab[pos_pf / 2 - 1],
								token_johab[pos_pf / 2], nth);
						}
						else
						{	  // '��' �� ���
							strncpy ((char*)str_first, (char*) str, ipos + 2);
							str_first[ipos + 2] = 0;
							if (ipos + 2 < i)
							{
								strncpy ((char*)str_middle, (char*) str + ipos + 2,
									i - ipos - 2);
								str_middle[i - ipos - 2] = 0;
							}
							//printf(" %s %s %s \n",str_first,str_middle,str_last);
							if (i > ipos + 2)
								treat_eogan_with_pf (str_first, str_middle,
									str_last,
									token_johab[ipos / 2],
									token_johab[ipos / 2 + 1],
									nth);
							else
								treat_eogan (str_first, str_last,
									token_johab[ipos / 2],
									token_johab[ipos / 2 + 1], nth);
						}
					}
					else if (i >= 4 && !strncmp ((char*)str + i - 2, "��", 2))
					{		  //�� ���� �����ð� ���°��
						if (i > 4 && !strncmp ((char*)str + i - 4, "��", 2))
						{
							//�� �տ� �� �� ������
							strncpy ((char*)str_middle, (char*) str + i - 4, len - i + 2);
							str_middle[len - i + 2] = 0;
							strncpy ((char*)str_first, (char*) str, i - 4);
							str_first[i - 4] = 0;
							if (is_belong_to_yongon_dic (str_first))
								save_result_yongon (nth, str_first, str_middle,
									str_last);
						}
						else
						{
							strncpy ((char*)str_first, (char*) str, i - 2);
							str_first[i - 2] = 0;
							//printf(" f=%s \n",str_first);
							strncpy ((char*)str_middle, (char*) str + i - 2, len - i + 2);
							str_middle[len - i + 2] = 0;
							if (is_belong_to_yongon_dic (str_first))
								save_result_yongon (nth, str_first, str_middle,
									str_last);
							if (is_chaon_plus_yi (str_first))
								save_result_yongon_plus_yi (nth, str_first,
									str_middle, str_last);
						}
					}
					else
					{		  // �� �̳� '(��)��' �� ���� ���, ��̴� ������ �ʴ� ���
						//printf("regular without pf   str=%s \n",str_last);
						strncpy ((char*)str_first, (char*) str, i);
						str_first[i] = 0;
						treat_eogan (str_first, str_last,
							token_johab[i / 2 - 1], token_johab[i / 2],
							nth);
					}
				}
				// ���Ĵ� ��� ��̰� ���� ���
				// �� �ұ�Ģ, �ʶ� �ұ�Ģ, �Ŷ� �ұ�Ģ
			}
			else
			if ((!strncmp ((char*)str + i - 2, "��", 2)
				&& !strncmp ((char*)str + i, "��", 2))
				||
				((!strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2))
				&& !strncmp ((char*)str + i, "�Ŷ�", 4))
				|| (!strncmp ((char*)str + i - 2, "��", 2)
				&& !strncmp ((char*)str + i, "�ʶ�", 4)))
			{
				strncpy ((char*)str_first, (char*) str, i - 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
				// ��̰� ���ϴ� ���
				// ��� ���ϴ� ���
				// �� �ұ�Ģ�� ��쵵 ��°���������� ���쵵 �ȴ�.
				// �߿�=��+��
			}
							  //���������� ��ħ����
			else if (fvl_prev[2] == 1 && fvl[0] == 13 &&
							  // ���������� ��/�� �� ���
				(fvl[1] == 14 || fvl[1] == 21)
				/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
			{				  // �� �ұ�Ģ
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				// ���⼭���ʹ� str_last �� �������־��h��.
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf("���ұ�Ģ:%s %s \n",str_first,str_last);
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (len - i == 2 && fvl_prev[2] == 1
				&& !strncmp ((char*)str + i, "��", 2))
			{
				//�� �ұ�Ģ : ������, �� ���� �� Ż������
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				str_last[2] = 0;
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (fvl_prev[2] == 1 && !strncmp ((char*)str + i, "��", 2))
			{
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[2] = 0;
				strncpy ((char*)str_last, (char*) str + i + 2, len - i - 2);
				str_last[len - i - 2] = 0;
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, NULL, str_last);
			}

		}					  // if(i>=2 && eomi_flag)
		// ���ĺ��ʹ� ��̰� ���ϴ� ���
		if (eomi_flag)
		{
			//printf("f=%d v=%d l=%d \n",fvl_prev[0],fvl_prev[1],fvl_prev[2]);
			// ������ ��/��/��/�� �� ��� ������ ������ �κ�+ ����+��� �� �и�(p.27)
			// ���� ��� ���� �� ���ϴ� �� ���ĵ� ��� ���ϴ� �� �Ǵµ�,
			// �� ��� ��̰� �ϴ� �� �ƴ� ���ϴ� �� �и��� �ǰ� ��� �� �� �Ǿ�� ��
			if (fvl[2] == 5 || fvl[2] == 9 || fvl[2] == 17 || fvl[2] == 19)
			{
				//printf("ma: ��,��,��,�� �� ���� \n");
							  // ��ħ�� ����
				convert_3_to_ks (fvl[0], fvl[1], 1, tmp);
							  // ���������� ��ħ�� ����, ��� ������ ���
				if (fvl_prev[2] == 1 &&
					!strncmp ((char*)tmp, "��", 2))
				{			  // �Ƹ��ٿ�, �Ƹ��ٿ� �� ���� �� �ұ�Ģ
					convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
					if (i >= 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
					str_first[i] = 0;
					fvl[2] = table_lcon_to_fcon[fvl[2]];
					convert_3_to_ks (fvl[2], 2, 1, tmp);
					strncpy ((char*)str_last, (char*) tmp, 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					//printf("���ұ�Ģ:%s %s \n",str_first,str_last);
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
				else if (check_belong_to_info_table_yongon (tmp, len_word))
				{
					// ��ħ�� ������ �κ��� ������ ����� �������� �� �� �ֳ�üũ
					//printf(" ���+ ��,��,��,�� \n");
					if (i > 0)
						strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					fvl[2] = table_lcon_to_fcon[fvl[2]];
					convert_3_to_ks (fvl[2], 2, 1, tmp);
					strncpy ((char*)str_last, (char*) tmp, 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						rule_regular (str_first, NULL, str_last, nth);
					}
				}
				if (fvl[2] == 5)
				{			  // ��
					convert_3_to_ks (fvl[0], fvl[1], 9, tmp);
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "��", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}

			}
			else if (!strncmp ((char*)str + i, "��", 2))
			{
				strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) "��", 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf( "%s  %s \n",str_first,str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					if (is_dongsa_with_ha (str_first))
						save_result_yongon (nth, str_first, NULL, str_last);
				}
			}
			else if (fvl[2] == 1 && fvl[1] == 7)
			{				  // 7=��
				strncpy ((char*)str_last, (char*) "��", 2);
				if (len > i - 2)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
				{
					if (i > 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					if (!strncmp ((char*)str + i, "��", 2))
					{		  // '��' �ұ�Ģ�� '��' �ұ�Ģ
						if (fvl_prev[2] == 9)
						{
							convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
							strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
							strncpy ((char*)str_first + i, (char*) "��", 2);
							str_first[i + 2] = 0;
							if (is_belong_to_yongon_dic (str_first))
							{
								save_result_yongon (nth, str_first, NULL,
									str_last);
							}
							  // �տ��� ������ ��� ��������
							strncpy ((char*)str_first, (char*) str, i);
						}
						else if (!strncmp ((char*)str_first + i - 2, "��", 2))
						{
							str_first[i] = 0;
							if (is_belong_to_yongon_dic (str_first))
							{
								save_result_yongon (nth, str_first, NULL,
									str_last);
							}
						}
					}
					if (!strncmp ((char*)str + i, "��", 2))
					{
							  // p.67
						strncpy ((char*)str_first + i, (char*) "Ǫ", 2);
						str_first[i + 2] = 0;
					}
					else
					{
						convert_3_to_ks (fvl[0], 27, 1, tmp);
						strncpy ((char*)str_first + i, (char*) tmp, 2);
						str_first[i + 2] = 0;
					}
					if (is_belong_to_yongon_dic (str_first))
						save_result_yongon (nth, str_first, NULL, str_last);
				}
			}
			else if (fvl[2] == 1 &&
				(fvl[1] == 3 || fvl[1] == 4 || fvl[1] == 10))
			{
				//       3=�� ,  4=�� , 10= ��
				//  ��) ���� = ����+��, ������ =����+� , �� = ����
				status_del = 0;
				if (check_belong_to_info_table (str + i, F_END_1))
				{			  // �� �� ������ ���
					strncpy ((char*)str_last, (char*) "��", 2);
					status_del = 1;
				}
							  // ��
				else if (check_belong_to_info_table (str + i, F_END_4) ||
					check_belong_to_info_table (str + i, F_END_5))
				{			  // ��
					strncpy ((char*)str_last, (char*) "��", 2);
					status_del = 1;
				}
				if (status_del == 1)
				{
					if (len >= i + 2)
						strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						strncpy ((char*)str_first, (char*) str, i + 2);
						str_first[i + 2] = 0;
						rule_regular (str_first, NULL, str_last, nth);
					}
				}
			}
			else if (fvl[2] == 1 && fvl[1] == 11)
			{				  // �� �� ������ ���
				convert_3_to_ks (fvl[0], 29, 1, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) tmp, 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				if (len - i - 2 >= 0)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
					if (is_belong_to_yongon_dic (str_first))
						save_result_yongon (nth, str_first, NULL, str_last);

			}
			else if (fvl[2] == 1 &&
				(fvl[1] == 14 || fvl[1] == 21 || fvl[1] == 11))
			{
				//                  14=��,  21=��,  11=��
							  // 13=��
				if (fvl[1] == 14)
					convert_3_to_ks (fvl[0], 13, 1, tmp);
							  // 20= ��
				else if (fvl[1] == 21)
					convert_3_to_ks (fvl[0], 20, 1, tmp);
				//printf("tmp=%s \n",tmp);
				if (check_belong_to_info_table (tmp, F_END_3))
				{			  // ��+ �� Ȥ�� ��+��
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "��", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					//printf( "%s  %s \n",str_first,str_last);
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
			}
			else if (fvl[1] == 15)
			{
				if (strncmp ((char*)str + i, "��", 2) && strncmp ((char*)str + i, "��", 2) &&
					strncmp ((char*)str + i, "��", 2))
				{			  // p.67
					strncpy ((char*)str_first, (char*) str, i);
					convert_3_to_ks (fvl[0], 18, 1, tmp);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "��", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
			}
		}
		if (!check_belong_to_info_table (str + i, F_JOSA2))
			josa_flag = 0;
		if (!check_belong_to_info_table (str + i, F_EOMI2))
			eomi_flag = 0;
	}
}


void
guess_token_to_ma (unsigned char *str, int nth, int position)
{
	char *p_tmp;
	int len;
	int josa_flag;
	int eomi_flag;
	int i;
	int j;
	int ipos;
	int pos_pf;
	int len_word;
	int len_total;
	unsigned char str_first[20];
	unsigned char str_middle[20];
	unsigned char str_last[20];
	unsigned char token_johab[20][3];
	unsigned char *fvl_prev;
	unsigned char *fvl;
	unsigned char *fvl_tmp;
	unsigned char tmp[9];
	unsigned char lcon;
	int status_del;
	int status_yongon_only;

	if (position == 2)
	{
		p_tmp = NULL;
		if ((p_tmp = strchr ((char*)str, '.')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, '?')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, '!')) != NULL)
		{
		}
		else if ((p_tmp = strchr ((char*)str, ',')) != NULL)
		{
		}

		if (p_tmp != NULL)
		{
			*p_tmp = 0;
			len = strlen ((char*)str);
		}
		else
			len = strlen ((char*)str);

	}
	else
	{
		len = strlen ((char*)str);
	}
	if (len == 0)
		return;

	// ���� ������ ��𿡸� �� �� �ִ� ������ ����ִ��� üũ �ؼ�
	// ��𿡸� �ü� ������ ü��+������ ���ɼ��� ����
	status_yongon_only = check_yongon_only (str, len);
	if (status_yongon_only)
		josa_flag = 0;
	else
		josa_flag = 1;
	eomi_flag = 1;

	len_total = token_to_johabs (str, len, token_johab);

	// ù������ ��̺κ��� ���ԵǾ� ���� ���ɼ��� �ִ°��� �׷��� ���� ��챸��
	// ��1) �� ��� -> �� +��, ���ٰ� -> �� + ���ٰ�
	// ��,�� �� ��������� ù���� ����� ��ħ�� �����Ƿ�
	// ù������ ���� �ȴ�.
	// ��2) �� ���ȴ�. ->�� + �� ����� ������ ��ħ���� ��, ��, ��, �� �� ������
	// ��� ��� '��' �� �� �� �����ǹǷ� ù������ ��̰� �ִٰ� ������ �� �ִ�.
	// ��3) �� = �� + �� �� �������
	// ��4) �� = �� + ��, �� = �� + �� , �� = �� + �� ���
	for (i = len - 2; i >= 0; i -= 2)
	{
		len_word = i / 2;
		fvl = &token_johab[i / 2][0];
		if (i >= 2)
		{
			fvl_prev = &token_johab[i / 2 - 1][0];
			if (josa_flag && check_belong_to_info_table (str + i, F_JOSA1))
			{
				strncpy ((char*)str_first, (char*) str, i);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				if (is_belong_to_josa_table (str_last) &&
					check_connection_cj (str_first, i, fvl_prev, str_last))
					save_result_chaon (nth, str_first, str_last);
			}
		}
		// ���⼭���� ���и�����
		// ���� ù�������� üũ�� �ʿ䰡 ���� ���
		if (i >= 2 && eomi_flag)
		{
			// ���� ��̰� ������ �ʴ� ���
			if (check_belong_to_info_table (str + i, F_EOMI1))
			{
				// ���������� ����� ù��° �ü��ִ���
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				//printf(" str_last=%s \n",str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					// ������������ ���ڿ��� ��̻���(���̺�)�� ��ϵǾ� �ִ���
					if (fvl_prev[2] == 22)
					{		  //��������  �� ��ħ�� �ִ� ���
						j = 0;
						while (i / 2 - 2 - j > 0
							&& token_johab[i / 2 - 2 - j][2] == 22)
						{
							j++;
						}
							  // ipos �� �� �� �� ������ ������
						ipos = i - 2 - 2 * j;
						if (strncmp ((char*)str + ipos, "��", 2))
						{	  // �� �� ��� ��̰� �ƴ�
							  // ������ �и�
							pos_pf = treat_pf (str, str_first, str_middle, token_johab[ipos / 2], ipos, i);
							save_result_yongon (nth, str_first, str_middle,
								str_last);
						}
						else
						{	  // '��' �� ���
							strncpy ((char*)str_first, (char*) str, ipos + 2);
							str_first[ipos + 2] = 0;
							if (ipos + 2 < i)
							{
								strncpy ((char*)str_middle, (char*) str + ipos + 2,
									i - ipos - 2);
								str_middle[i - ipos - 2] = 0;
							}
							//printf(" %s %s %s \n",str_first,str_middle,str_last);
							if (i > ipos + 2)
								save_result_yongon (nth, str_first, str_middle,
									str_last);
							else
								save_result_yongon (nth, str_first, NULL,
									str_last);
						}
					}
					else if (i >= 4 && !strncmp ((char*)str + i - 2, "��", 2))
					{		  //�� ���� �����ð� ���°��
						if (i > 4 && !strncmp ((char*)str + i - 4, "��", 2))
						{
							//�� �տ� �� �� ������
							strncpy ((char*)str_middle, (char*) str + i - 4, len - i + 2);
							str_middle[len - i + 2] = 0;
							strncpy ((char*)str_first, (char*) str, i - 4);
							str_first[i - 4] = 0;
							save_result_yongon (nth, str_first, str_middle,
								str_last);
						}
						else
						{
							strncpy ((char*)str_first, (char*) str, i - 2);
							str_first[i - 2] = 0;
							strncpy ((char*)str_middle, (char*) str + i - 2, len - i + 2);
							str_middle[len - i + 2] = 0;
							save_result_yongon (nth, str_first, str_middle,
								str_last);
							if (!strncmp ((char*)str + i - 2, "��", 2))
								save_result_yongon_plus_yi (nth, str_first,
									str_middle, str_last);
						}
					}
					else
					{		  // �� �̳� '(��)��' �� ���� ���, ��̴� ������ �ʴ� ���
						strncpy ((char*)str_first, (char*) str, i);
						str_first[i] = 0;
						save_result_yongon (nth, str_first, NULL, str_last);
						if (!strncmp ((char*)str + i - 2, "��", 2))
							save_result_yongon_plus_yi (nth, str_first, NULL,
								str_last);
					}
				}
				// ���Ĵ� ��� ��̰� ���� ���
				// �� �ұ�Ģ, �ʶ� �ұ�Ģ, �Ŷ� �ұ�Ģ
			}
			else
			if ((!strncmp ((char*)str + i - 2, "��", 2)
				&& !strncmp ((char*)str + i, "��", 2))
				||
				((!strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2)
				|| !strncmp ((char*)str + i - 2, "��", 2))
				&& !strncmp ((char*)str + i, "�Ŷ�", 4))
				|| (!strncmp ((char*)str + i - 2, "��", 2)
				&& !strncmp ((char*)str + i, "�ʶ�", 4)))
			{
				strncpy ((char*)str_first, (char*) str, i - 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
				// ��̰� ���ϴ� ���
				// ��� ���ϴ� ���
				// �� �ұ�Ģ�� ��쵵 ��°���������� ���쵵 �ȴ�.
				// �߿�=��+��
			}
							  //���������� ��ħ����
			else if (fvl_prev[2] == 1 && fvl[0] == 13 &&
							  // ���������� ��/�� �� ���
				(fvl[1] == 14 || fvl[1] == 21)
				/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
			{				  // �� �ұ�Ģ
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				// ���⼭���ʹ� str_last �� �������־��h��.
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf("���ұ�Ģ:%s %s \n",str_first,str_last);
				save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (len - i == 2 && fvl_prev[2] == 1
				&& !strncmp ((char*)str + i, "��", 2))
			{
				//�� �ұ�Ģ : ������, �� ���� �� Ż������
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				str_last[2] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (fvl_prev[2] == 1 && !strncmp ((char*)str + i, "��", 2))
			{
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[2] = 0;
				strncpy ((char*)str_last, (char*) str + i + 2, len - i - 2);
				str_last[len - i - 2] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
			}

		}					  // if(i>=2 && eomi_flag)
		// ���ĺ��ʹ� ��̰� ���ϴ� ���

		if (eomi_flag)
		{
			//printf("f=%d v=%d l=%d \n",fvl_prev[0],fvl_prev[1],fvl_prev[2]);
			// ������ ��/��/��/�� �� ��� ������ ������ �κ�+ ����+��� �� �и�(p.27)
			// ���� ��� ���� �� ���ϴ� �� ���ĵ� ��� ���ϴ� �� �Ǵµ�,
			// �� ��� ��̰� �ϴ� �� �ƴ� ���ϴ� �� �и��� �ǰ� ��� �� �� �Ǿ�� ��
			if (fvl[2] == 5 || fvl[2] == 9 || fvl[2] == 17 || fvl[2] == 19)
			{
				//printf("ma: ��,��,��,�� �� ���� \n");
							  // ��ħ�� ����
				convert_3_to_ks (fvl[0], fvl[1], 1, tmp);
							  // ���������� ��ħ�� ����, ��� ������ ���
				if (fvl_prev[2] == 1 &&
					!strncmp ((char*)tmp, "��", 2))
				{			  // �Ƹ��ٿ�, �Ƹ��ٿ� �� ���� �� �ұ�Ģ
					convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
					if (i >= 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
					str_first[i] = 0;
					fvl[2] = table_lcon_to_fcon[fvl[2]];
					convert_3_to_ks (fvl[2], 2, 1, tmp);
					strncpy ((char*)str_last, (char*) tmp, 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					//printf("���ұ�Ģ:%s %s \n",str_first,str_last);
					if (is_belong_to_eomi_table (str_last))
					{
						save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
				else if (check_belong_to_info_table_yongon (tmp, len_word))
				{
					// ��ħ�� ������ �κ��� ������ ����� �������� �� �� �ֳ�üũ
					//printf(" ���+ ��,��,��,�� \n");
					if (i > 0)
						strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					fvl[2] = table_lcon_to_fcon[fvl[2]];
					convert_3_to_ks (fvl[2], 2, 1, tmp);
					strncpy ((char*)str_last, (char*) tmp, 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						save_result_yongon (nth, str_first, NULL, str_last);
						if (strncpy ((char*)tmp, (char*) "��", 2))
							save_result_yongon_plus_yi (nth, str_first, NULL,
								str_last);
					}
				}

			}
			else if (!strncmp ((char*)str + i, "��", 2))
			{
				strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) "��", 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf( "%s  %s \n",str_first,str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					save_result_yongon (nth, str_first, NULL, str_last);
				}
			}
			else if (fvl[2] == 1 && fvl[1] == 7)
			{				  // 7=��
				strncpy ((char*)str_last, (char*) "��", 2);
				if (len > i - 2)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
				{
					if (i > 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					if (!strncmp ((char*)str + i, "��", 2))
					{		  // '��' �ұ�Ģ�� '��' �ұ�Ģ
						if (fvl_prev[2] == 9)
						{
							convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
							strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
							strncpy ((char*)str_first + i, (char*) "��", 2);
							str_first[i + 2] = 0;
							save_result_yongon (nth, str_first, NULL, str_last);
							  // �տ��� ������ ��� ��������
							strncpy ((char*)str_first, (char*) str, i);
						}
						else if (!strncmp ((char*)str_first + i - 2, "��", 2))
						{
							str_first[i] = 0;
							save_result_yongon (nth, str_first, NULL, str_last);
						}
					}
					if (!strncmp ((char*)str + i, "��", 2))
					{
							  // p.67
						strncpy ((char*)str_first + i, (char*) "Ǫ", 2);
						str_first[i + 2] = 0;
					}
					else
					{
						convert_3_to_ks (fvl[0], 27, 1, tmp);
						strncpy ((char*)str_first + i, (char*) tmp, 2);
						str_first[i + 2] = 0;
					}
					save_result_yongon (nth, str_first, NULL, str_last);
				}
			}
			else if (fvl[2] == 1 &&
				(fvl[1] == 3 || fvl[1] == 4 || fvl[1] == 10))
			{
				//       3=�� ,  4=�� , 10= ��
				//  ��) ���� = ����+��, ������ =����+� , �� = ����
				status_del = 0;
				if (check_belong_to_info_table (str + i, F_END_1))
				{			  // �� �� ������ ���
					strncpy ((char*)str_last, (char*) "��", 2);
					status_del = 1;
				}
							  // ��
				else if (check_belong_to_info_table (str + i, F_END_4) ||
					check_belong_to_info_table (str + i, F_END_5))
				{			  // ��
					strncpy ((char*)str_last, (char*) "��", 2);
					status_del = 1;
				}
				if (status_del == 1)
				{
					if (len >= i + 2)
						strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						strncpy ((char*)str_first, (char*) str, i + 2);
						str_first[i + 2] = 0;
						save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
			}
			else if (fvl[2] == 1 && fvl[1] == 11)
			{				  // �� �� ������ ���
				convert_3_to_ks (fvl[0], 29, 1, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) tmp, 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "��", 2);
				if (len - i - 2 >= 0)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
					save_result_yongon (nth, str_first, NULL, str_last);

			}
			else if (fvl[2] == 1 &&
				(fvl[1] == 14 || fvl[1] == 21 || fvl[1] == 11))
			{
				//                  14=��,  21=��,  11=��
							  // 13=��
				if (fvl[1] == 14)
					convert_3_to_ks (fvl[0], 13, 1, tmp);
							  // 20= ��
				else if (fvl[1] == 21)
					convert_3_to_ks (fvl[0], 20, 1, tmp);
				if (check_belong_to_info_table (tmp, F_END_3))
				{			  // ��+ �� Ȥ�� ��+��
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "��", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
			}
			else if (fvl[1] == 15)
			{
				if (strncmp ((char*)str + i, "��", 2) && strncmp ((char*)str + i, "��", 2) &&
					strncmp ((char*)str + i, "��", 2))
				{			  // p.67
					strncpy ((char*)str_first, (char*) str, i);
					convert_3_to_ks (fvl[0], 18, 1, tmp);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "��", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
			}
		}
		if (!check_belong_to_info_table (str + i, F_JOSA2))
			josa_flag = 0;
		if (!check_belong_to_info_table (str + i, F_EOMI2))
			eomi_flag = 0;
	}
}