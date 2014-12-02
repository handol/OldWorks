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
		(!strncmp ((char*)chaon, "내", 2) || !strncmp ((char*)chaon, "네", 2)
		|| !strncmp ((char*)chaon, "제", 2)))
	{
		if (!strncmp ((char*)str, "가", 2) || !strncmp ((char*)str, "게", 2))
			return 1;
		else
			return 0;
	}
	else if (!strcmp ((char*)(char*)chaon, "나") || !strncmp ((char*)chaon, "너", 2)
		|| !strcmp ((char*)chaon, "저"))
	{
		if (!strcmp ((char*)str, "가"))
			return 0;
	}
	else if (!strncmp ((char*)str, "이", 2) ||
		!strncmp ((char*)str, "은", 2) || !strncmp ((char*)str, "을", 2) ||
		!strncmp ((char*)str, "과", 2) || !strncmp ((char*)str, "아", 2))
	{
		if (fvl[2] == 1)
			return 0;		  // 받침이 없는 경우는 불가능
		else
			return 1;
	}
	else if (!strncmp ((char*)str, "가", 2) || !strncmp ((char*)str, "는", 2) ||
		!strncmp ((char*)str, "를", 2) || !strncmp ((char*)str, "와", 2) ||
		!strncmp ((char*)str, "고", 2) || !strncmp ((char*)str, "다", 2) ||
		!strncmp ((char*)str, "든", 2) || !strncmp ((char*)str, "라", 2) ||
		!strncmp ((char*)str, "랑", 2) || !strncmp ((char*)str, "며", 2) ||
		!strncmp ((char*)str, "야", 2) || !strncmp ((char*)str, "여", 2))
	{
		if ((int) fvl[2] > 1)
			return 0;
		return 1;
	}
	else if (!strncmp ((char*)str, "로", 2))
	{
		if (fvl[2] == 1 || fvl[2] == 9)
			return 1;		  // 모음이나 ㄹ 로 끝나야만
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
	// 용언에서만 사용되는 음절,
	// 만일 이 음절중 하나를 가지고 있으면 체언탐색을 하지 않아도 된다.
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
	// 원래는 이진 탐색을 해야하지만 , 생묘네 사전에 조사나 어미로 등록되지 않은
	// 단어가 많아서 필요한 경우 손으로  단어를 추가해야 하기 때문에
	// 일단 순차적인 탐색을 수행하도록 코딩
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
	// 위와 마찬가지 이유로 일단은 순차적인 탐색
	// 이진 탐색을 하려면 단어가 순서대로 배열되어야 함
	int i;
	int len;
	len = strlen ((char*)str);
	if (len == 2)
	{
		if (!strncmp ((char*)str, "ㄴ", 2) || !strncmp ((char*)str, "ㄹ", 2)
			|| !strncmp ((char*)str, "ㅁ", 2) || !strncmp ((char*)str, "ㅂ", 2))
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


// 사전에서 이진탐색을 하기 위한 함수
// 같은 크기의 단어로 구성된 사전에서는 offset 을 인덱스로 사용하고
// 스트링 값을 인덱스에 저장된 값으로 생각하면
// strncmp ((char*)strcmp) 로 이진탐색을 할 수 있다.
int
b_search (int kind, FILE * fp, long start, long end, unsigned char *str, int len)
{
	char str_read[20];
	long half;
	int val;
	int no_of_word;
	if (start == end - 2 * len)
	{						  // 한글은 2바이트이므로
		fseek (fp, start, SEEK_SET);
		fread (str_read, len, 1, fp);
		/* if(!strncmp(str,"오",2) ) {
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
			//if(!strncmp(str,"되",2)) printf("str_read=%s \n",str_read);
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
	// 끝 음절이 체언의 끝음절이 될 수 있는지 체크
	// 사전탐색 횟수를 줄이기 위해
	if (len / 2 > 10)
		return 1;			  //일단 이보다  긴 글자는 무조건 체언으로
	sprintf (fname, "chaon_%d.dic", len / 2);
	fp = fopen (fname, "r");

	result =
		b_search (CHAON, fp, 0, initial_end_chaon[len / 2 - 1] * len, str, len);
	if (result == 0)
	{
		p_str = str + len - 2;
		if (!strncmp ((char*)p_str, "적", 2) || !strncmp ((char*)p_str, "들", 2) ||
			!strncmp ((char*)p_str, "등", 2) || !strncmp ((char*)p_str, "등", 2))
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
	if (!strncmp ((char*)str, "없", 2))
		printf ("str =%s \n");
	if (len == 2)
		if (check_belong_to_info_table (str, F_YONGON1))
			return 1;
	else
		return 0;
	if (!check_belong_to_info_table_yongon (str + len - 2, len / 2))
		return 0;
	// 체언의 경우와 마찬가지. 용언과 체언을 제외한 나머지는
	// 뽁Ｃ銹맑側� 적으므로 해당함수를 호출하기 바로전에 체크해도 무방

	if (len / 2 > 6)
		return 0;			  // 긴 글자는 용언에서 제외
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
		return 0;			  // 긴 글자는  제외
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
		return 0;			  // 긴 글자는  제외
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
		return 0;			  // 긴 글자는  제외
	sprintf (fname, "gamtansa_%d.dic", len / 2);
	fp = fopen (fname, "r");
	result =
		b_search (GAMTANSA, fp, 0, initial_end_gamtansa[len / 2 - 1] * len, str,
		len);
	fclose (fp);
	return result;
}


// 체언+ 서술격조사의 경우 서술격 조사가 어미변화와 같은 변화를 함
int
is_chaon_plus_yi (unsigned char *str_org)
{
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	if (len <= 2)
		return 0;
	if (strncmp ((char*)str + len - 2, "이", 2))
		return 0;
	str[len - 2] = 0;
	return is_belong_to_chaon_dic (str);
}


// 체언+하다로 된 동사는 사전에 등록되지 않은 경우가 많다
// 그러나 이경우도 동사이므로 고려해야 한다.
// 체언+스럽다 등도 고려해야 하지만 일단은 보류
int
is_dongsa_with_ha (unsigned char *str_org)
{
	// 가끔 황당한 결과를 준다.
	// 예를 들면 오해야 라는 말을
	//  오하 어다 로 나누어질 수 있는데,
	// 오하는 다시 오+하 로 나누어지고 오 가 체언이 될 수 있으므로
	// 마치 오하다 라는 동사로 해석
	// 이런 일을 막으려면 하다와 결합해서 동사가 될 수 있는지를
	// 표시해야 하는데 그러면 일이 너무 복잡..
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	if (strncmp ((char*)str + len - 2, "하", 2))
		return 0;
	if (len == 2)
		return 1;			  // 하 동사
	str[len - 2] = 0;
							  // 체언+하 동사
	return is_belong_to_chaon_dic (str);
}


// 죽이다 와 같이 어간이 이 로 끝나는 동사
// 이러한 동사는  선어말어미 결합시 이+었-> 였 으로 되기 때문에
// 이 함수는 선어말어미 탐색시 필요
int
is_dongsa_with_yi (unsigned char *str_org)
{
	int len;
	unsigned char str[20];
	strcpy ((char*)str, (char*) str_org);

	len = strlen ((char*)str);
	strncpy ((char*)str + len, (char*) "이", 2);
	str[len + 2] = 0;
	return is_belong_to_yongon_dic (str);
}


//선어말 어미 분리
int
treat_pf (unsigned char *str, unsigned char *str_first,
unsigned char *str_middle,
unsigned char *fvl_tmp, int ipos, int len)
{
	unsigned char tmp[9];

	if (!strncmp ((char*)str + ipos, "었", 2) || !strncmp ((char*)str + ipos, "겠", 2))
	{						  // 었, 겠

		if (ipos > 2 && !strncmp ((char*)str + ipos - 2, "시", 2))
		{					  // 어간이 두음절이상
			// 선어말 어미 시는 독립으로 나오는 경우외에는 다음에 었 이나 겠 이 온다.
			if (ipos > 4 && !strncmp ((char*)str + ipos - 4, "으", 2))
			{				  // 시 앞에 으 는 선어말어미
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
		{					  // '시'나 '으시'가 아닌 경우 ipos 부터 선어말 어미
			strncpy ((char*)str_middle, (char*) str + ipos, len - ipos);
			str_middle[len - ipos] = 0;
			strncpy ((char*)str_first, (char*) str, ipos);
			str_first[ipos] = 0;
			return ipos;
		}
		return ipos;
	}
	else if (!strncmp ((char*)str + ipos, "았", 2) || !strncmp ((char*)str + ipos, "였", 2))
	{						  //'았", '였' 앞에는 시가 오지 못 함
		strncpy ((char*)str_middle, (char*) str + ipos, len - ipos);
		str_middle[len - ipos] = 0;
		strncpy ((char*)str_first, (char*) str, ipos);
		str_first[ipos] = 0;
		return ipos;
	}
	else if (!strncmp ((char*)str + ipos, "셨", 2))
	{						  // '셨' 앞에는 '으' 가 올 수 있음
		// 일단 선어밀어미로 간주한 후 후에 부시다와 같은 경우 즉 선어말 어미가 시로
		// 시작되는 경우 재검토

		if (ipos > 2 && !strncmp ((char*)str + ipos - 2, "으", 2))
		{					  //앞에 으 는 선어말어미
			strncpy ((char*)str_middle, (char*) str + ipos - 2, len - ipos + 2);
			str_middle[len - ipos + 2] = 0;
			strncpy ((char*)str_first, (char*) str, ipos - 2);
			str_first[ipos - 2] = 0;
			return ipos - 4;
		}
		else
		{
			strncpy ((char*)str_middle, (char*) "시었", 4);
			strncpy ((char*)str_middle + 4, (char*) str + ipos + 2, len - ipos - 2);
			str_middle[len - ipos + 2] = 0;
			strncpy ((char*)str_first, (char*) str, ipos);
			str_first[ipos] = 0;
			return ipos;
		}
	}
	else if (!strncmp ((char*)str + ipos, "했", 2))
	{
		strncpy ((char*)str_middle, (char*) "았", 2);
		strncpy ((char*)str_middle + 2, (char*) str + ipos + 2, len - ipos - 2);
		str_middle[len - ipos] = 0;
		strncpy ((char*)str_first, (char*) str, ipos);
		strncpy ((char*)str_first + ipos, (char*) "하", 2);
		str_first[ipos + 2] = 0;
		return ipos + 2;
	}
							  // ㅏ, ㅐ
	else if (fvl_tmp[1] == 3 || fvl_tmp[1] == 4 ||
		fvl_tmp[1] == 7 ||	  // ㅓ,
							  // ㅘ, ㅙ
		fvl_tmp[1] == 14 || fvl_tmp[1] == 15 ||
		fvl_tmp[1] == 11 || fvl_tmp[1] == 21)
	{						  // ㅕ, ㅝ,
		convert_3_to_ks (fvl_tmp[0], fvl_tmp[1], 1, tmp);
		strncpy ((char*)str_first, (char*) str, ipos);
		strncpy ((char*)str_first + ipos, (char*) tmp, 2);
		str_first[ipos + 2] = 0;
		strncpy ((char*)str_middle, (char*) "ㅆ", 2);
		strncpy ((char*)str_middle + 2, (char*) str + ipos + 2, len - ipos - 2);
		str_middle[len - ipos] = 0;
		return ipos;
	}
}


void
print_result_chaon (unsigned char *str_first, unsigned char *str_last)
{
	printf ("(%s/체언)+(%s/조사) \n", str_first, str_last);
}


void
print_result_yongon (unsigned char *str_first,
unsigned char *str_middle, unsigned char *str_last)
{

	printf ("(%s/용언)", str_first);
	if (str_middle)
		printf ("+(%s/선어말어미)", str_middle);
	printf ("+(%s/어미) \n", str_last);
}


void
print_result_yongon_plus_yi (unsigned char *str_first,
unsigned char *str_middle,
unsigned char *str_last)
{
	printf ("%s/(체언+서술격조사)", str_first);
	if (str_middle)
		printf ("+(%s/선어말어미)", str_middle);
	printf ("+(%s/어미) \n", str_last);
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
			printf ("(%s/부사) \n", l_result_ma->single);
		}
		else if (type == S_KWANHYUNGSA)
		{
			printf ("(%s/관형사) \n", l_result_ma->single);
		}
		else if (type == S_CHAON)
		{
			printf ("(%s/체언) \n", l_result_ma->single);
		}
		else if (type == S_GAMTANSA)
		{
			printf ("(%s/감탄사) \n", l_result_ma->single);
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

							  // 먹이다, 죽이다와 같이 2중으로 생각할 수
	if (is_chaon_plus_yi (str_first))
		save_result_yongon_plus_yi (nth, str_first, str_middle, str_last);
							  // else 를 사용하지 않음
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
	{						  // 종성이 ㄹ , 초성이 ㅇ
		// 어간이 변형(그러나 어미는 유지)
		// ㄷ 불규칙 : 걸어= 걷+어  즉 ㄷ -> ㄹ 로 바뀌는 경우 p.61
		if (check_belong_to_info_table (syllable_last, F_BUL_D))
		{
							  // 받침을 ㄷ 으로
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
		//              ( !strncmp(str+i,"어",2) || !strncmp(str+i,"으",2) ) ){
		// ㅅ 불규칙 : 저어= 젓 + 어 p.62
		if (check_belong_to_info_table (syllable_last, F_BUL_S))
		{
							  // 받침을 ㅅ 으로
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 21, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
	}
	else if (fvl_prev[2] == 1 &&
		(fvl[0] == 4 || fvl[0] == 7 || fvl[0] == 8 || fvl[0] == 9))
	{
		// ㅎ 불규칙 : 파라니까 = 파랗+니까   ㅎ 이 ㄴ,ㄹ,ㅁ,ㅂ 앞에서 탈락?
		if (check_belong_to_info_table (syllable_last, F_BUL_H))
		{
							  // 받침을 ㅎ 으로
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 29, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
		if (fvl[0] == 4
			&& check_belong_to_info_table (syllable_last, F_BUL_DEL_L))
		{
			// ㄹ 탈락 : 가니=갈+니
							  // 받침을 ㄹ 으로
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 9, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, NULL, str_last);
		}
	}
}


// 선어말 어미가 있는 경우 없는 경우와 몇가지 차이가 있다.
// 예를 들어 선어말어미가 있는 경우 ㅎ불규칙이나 ㄹ 탈락이 올 수 없다.
// 또 한 고왔었다 와 같이 ㅂ 불규칙인 경우라도 어미가 이미 고정된 형태이다.
// 선어말 어미가 없는 고와서 같은 경우는 어미가 '아서' 이므로 '와' 를 탐색하는
// 시점에서는 어미를 결정할 수 없다.
// 따라서 불규칙에 ㅂ 불규칙을 포함시켜도 된다.
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
	if (!strncmp ((char*)str_middle, "였", 2))
	{
		// '하'+'였'
		if (is_dongsa_with_ha (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);
		// '였' = '이'+'었'
		if (is_dongsa_with_yi (str_first))
		{
			strncpy ((char*)str_first + len, (char*) "이", 2);
			str_first[len + 2] = 0;
			strncpy ((char*)str_middle, (char*) "었", 2);
			save_result_yongon (nth, str_first, str_middle, str_last);
		}
	}
	else if (fvl_prev[2] == 9 && fvl[0] == 13)
	{						  // 종성이 ㄹ , 초성이 ㅇ
		// 어간이 변형(그러나 어미는 유지)
		// ㄷ 불규칙 : 걸어= 걷+어  즉 ㄷ -> ㄹ 로 바뀌는 경우 p.61
		if (check_belong_to_info_table (syllable_last, F_BUL_D))
		{
							  // 받침을 ㄷ 으로
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
		//              ( !strncmp(str+i,"어",2) || !strncmp(str+i,"으",2) ) ){
		// ㅅ 불규칙 : 저어= 젓 + 어 p.62
		if (check_belong_to_info_table (syllable_last, F_BUL_S))
		{
							  // 받침을 ㅅ 으로
			convert_3_to_ks (fvl_prev[0], fvl_prev[1], 21, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			if (is_belong_to_yongon_dic (str_first))
				save_result_yongon (nth, str_first, str_middle, str_last);
		}
							  // 현재음절이 와/워 인 경우
		if ((fvl[1] == 14 || fvl[1] == 21)
			/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
		{					  // ㅂ 불규칙
			if (len >= 4)
			{
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				strcpy ((char*)str_tmp, (char*) str_first);
				strncpy ((char*)str_first + len - 4, (char*) tmp, 2);
				str_first[len - 2] = 0;
				//printf("ㅂ 불규칙 %s \n", str_first);
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, str_middle, str_last);
			}
			// 오다 -> 와 처럼 축약의 경우
			if (fvl[1] == 14)  // ㅘ
							  // ㅗ
				convert_3_to_ks (fvl[0], 13, 1, tmp);
			else
							  // ㅜ
				convert_3_to_ks (fvl[0], 20, 1, tmp);
			strncpy ((char*)str_tmp + len - 2, (char*) tmp, 2);
			str_tmp[len];
			if (is_belong_to_yongon_dic (str_tmp))
				save_result_yongon (nth, str_tmp, str_middle, str_last);

		}
		// 선어말 어미가 있는 경우 ㅎ 이나 ㄹ 이 탈락되는 일이 없다.
		// 그러나 파랗다 -> 파랬었다. 와 같이 어간,어미가 동시에 변하는 경우는 있음
	}
	else if (fvl_prev[1] == 4 && fvl_prev[2] == 22 && len >= 4)
	{
		// ㅐ 로 끝나야 하고 뉼굶低뻬儲肩� ㅆ 이 와야함
		// ㅎ 불규칙 : �
		if (check_belong_to_info_table (syllable_last, F_BUL_H))
		{
			//        convert_3_to_ks(fvl_prev[0],fvl_prev[1],29,tmp); // 받침을 ㅎ 으로
			// 이건 어간 복원이 복잡
			// 노랬다 동그랬다 빨갰다 벌갰다. 전음절을 조사하기 전에는 어려움�
			//        strncpy(str_first+len-2, (char*)tmp,2);
			//        if( is_belong_to_yongon_dic( str_first) )
			save_result_yongon (nth, str_first, str_middle, str_last);
		}
	}
	else if (fvl[1] == 11)
	{						  // ㅕ + ㅆ  -> ㅣ + 었
							  // ㅣ
		convert_3_to_ks (fvl[0], 29, 1, tmp);
		strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
		str_first[len] = 0;
		strncpy ((char*)str_middle, (char*) "었", 2);
		if (is_belong_to_yongon_dic (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);

	}
	else if (fvl[1] == 7)
	{						  // ㅓ 모음인 경우 ㅡ 탈락 고려
		if (!strncmp ((char*)syllable_last, "러", 2))
		{					  // '르' 불규칙과 '러' 불규칙
			if (fvl_prev[2] == 9)
			{
				strcpy ((char*)str_tmp, (char*) str_first);
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
				strncpy ((char*)str_first + len - 4, (char*) tmp, 2);
				strncpy ((char*)str_first + len - 2, (char*) "르", 2);
				if (is_belong_to_yongon_dic (str_first))
				{
					strncpy ((char*)str_middle, (char*) "었", 2);
					save_result_yongon (nth, str_first, str_middle, str_last);
				}
				strncpy ((char*)str_first, (char*) str_tmp, len);
			}
			else if (!strncmp ((char*)str_first + len - 4, "르", 2))
			{
				str_first[len - 2] = 0;
				if (is_belong_to_yongon_dic (str_first))
				{
					strncpy ((char*)str_middle, (char*) "었", 2);
					save_result_yongon (nth, str_first, str_middle, str_last);
				}
			}
		}
		if (!strncmp ((char*)str_first, "퍼", 2))
							  // p.67
			strncpy ((char*)str_first, (char*) "푸", 2);
		else
		{
			convert_3_to_ks (fvl[0], 27, 1, tmp);
			strncpy ((char*)str_first + len - 2, (char*) tmp, 2);
			strncpy ((char*)str_middle, (char*) "었", 2);
		}
		if (is_belong_to_yongon_dic (str_first))
			save_result_yongon (nth, str_first, str_middle, str_last);

	}
	else if (fvl[1] == 15)
	{						  // ㅙ +ㅆ = ㅚ + 었
		if (strncmp ((char*)syllable_last, "꾀", 2) && strncmp ((char*)syllable_last, "뇌", 2)
			&& strncmp ((char*)syllable_last, "외", 2))
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
	if (strncmp ((char*)str_middle, "였", 2))
		rule_regular (str_first, str_middle, str_last, nth);
	rule_irregular_with_pf (str_first, str_middle, str_last, fvl_prev, fvl,
		nth);
}


// 용언이 명사형이 되는 경우
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

	if (!strncmp ((char*)p_str + len - 2, "기", 2) || fvl_prev[2] != 17)
		return 0;			  // ㅁ 받침
	if (len >= 4)
	{
		if (!strncmp ((char*)p_str + len - 2, "기", 2)
			|| !strncmp ((char*)p_str + len - 2, "음", 2))
		{
			p_str[len - 2] = 0;
			if (is_belong_to_yongon_dic (p_str))
				return 1;
			else
				return 0;
		}
		else if (!strncmp ((char*)p_str + len - 2, "움", 2))
		{					  // 치우다-> 치움 , 덥다 -> 더움
			strncpy ((char*)p_str + len - 2, (char*) "우", 2);
			if (is_belong_to_yongon_dic (p_str))
				return 1;
			else if (fvl_pre_prev[2] == 1)
			{
							  // ㅂ 추가
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
			if (!strncmp ((char*)tmp, "하", 2) || !strncmp ((char*)tmp, "이", 2))
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
		if (!strncmp ((char*)tmp, "하", 2) || !strncmp ((char*)tmp, "이", 2))
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
	//printf(" 단일어 분석 이전 \n");
	// 먼저 단일어 중 부사인지 체크
	if (check_belong_to_info_table_busa (str + len - 2, len / 2))
	{
		if (is_belong_to_busa_dic (str))
			save_result_single_word (nth, str, S_BUSA);
		else
		{
			if (!strncmp ((char*)str + len - 2, "이", 2))
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

	//printf(" 단일어분석 이후 \n");

	// 먼저 음절중 용언에만 올 수 있는 음절이 들어있는지 체크 해서
	// 용언에만 올수 있으면 체언+조사의 가능성은 없음
	status_yongon_only = check_yongon_only (str, len);
	if (status_yongon_only)
		josa_flag = 0;
	else
		josa_flag = 1;
	eomi_flag = 1;

	len_total = token_to_johabs (str, len, token_johab);

	// 첫음절에 어미부분이 포함되어 있을 가능성이 있는경우와 그렇지 않은 경우구분
	// 예1) 갈 사람 -> 가 +ㄹ, 간다고 -> 가 + ㄴ다고
	// ㄴ,ㄹ 이 어미이지만 첫음절 용언의 받침이 없으므로
	// 첫음절에 포함 된다.
	// 예2) 서 버렸다. ->서 + 어 용언으 모음이 받침없는 ㅏ, ㅓ, ㅐ, ㅔ 로 끝나는
	// 경우 어미 '아' 나 어 가 생략되므로 첫음절에 어미가 있다고 생각할 수 있다.
	// 예3) 써 = 쓰 + 어 ㅡ 축약현상
	// 예4) 펴 = 피 + 어, 꿔 = 꾸 + 어 , 와 = 오 + 아 등등
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
		// 여기서부터 용언분리과정
		// 먼저 첫음절까지 체크할 필요가 없는 경우
		if (i >= 2 && eomi_flag)
		{
			// 먼저 어미가 변하지 않는 경우
			if (check_belong_to_info_table (str + i, F_EOMI1))
			{
				// 현재음절이 어미의 첫번째 올수있는지
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				//printf(" str_last=%s \n",str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					// 현재음절이후 문자열이 어미사전(테이블)에 등록되어 있는지
					if (fvl_prev[2] == 22)
					{		  //전음절에  ㅆ 받침이 있는 경우
						j = 0;
						while (i / 2 - 2 - j > 0
							&& token_johab[i / 2 - 2 - j][2] == 22)
						{
							j++;
						}
							  // ipos 는 ㅆ 이 들어간 음절의 시작점
						ipos = i - 2 - 2 * j;
						if (strncmp ((char*)str + ipos, "있", 2))
						{	  // 있 은 선어말 어미가 아님
							  // 선어말어미 분리
							pos_pf = treat_pf (str, str_first, str_middle, token_johab[ipos / 2], ipos, i);
							//            fvl_tmp=token_johab[ipos/2];
							//printf(" %s %s %s %d \n",str_first,str_middle,str_last, pos_pf);
							treat_eogan_with_pf (str_first, str_middle,
								str_last,
								token_johab[pos_pf / 2 - 1],
								token_johab[pos_pf / 2], nth);
						}
						else
						{	  // '있' 의 경우
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
					else if (i >= 4 && !strncmp ((char*)str + i - 2, "시", 2))
					{		  //ㅆ 없이 직접시가 오는경우
						if (i > 4 && !strncmp ((char*)str + i - 4, "으", 2))
						{
							//시 앞에 으 는 선어말어미
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
					{		  // ㅆ 이나 '(으)시' 가 없는 경우, 어미는 변하지 않는 경우
						//printf("regular without pf   str=%s \n",str_last);
						strncpy ((char*)str_first, (char*) str, i);
						str_first[i] = 0;
						treat_eogan (str_first, str_last,
							token_johab[i / 2 - 1], token_johab[i / 2],
							nth);
					}
				}
				// 이후는 선어말 어미가 없는 경우
				// 여 불규칙, 너라 불규칙, 거라 불규칙
			}
			else
			if ((!strncmp ((char*)str + i - 2, "하", 2)
				&& !strncmp ((char*)str + i, "여", 2))
				||
				((!strncmp ((char*)str + i - 2, "가", 2)
				|| !strncmp ((char*)str + i - 2, "오", 2)
				|| !strncmp ((char*)str + i - 2, "있", 2)
				|| !strncmp ((char*)str + i - 2, "자", 2))
				&& !strncmp ((char*)str + i, "거라", 4))
				|| (!strncmp ((char*)str + i - 2, "오", 2)
				&& !strncmp ((char*)str + i, "너라", 4)))
			{
				strncpy ((char*)str_first, (char*) str, i - 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
				// 어미가 변하는 경우
				// 어간도 변하는 경우
				// ㅂ 불규칙의 경우도 둘째음절까지만 살펴도 된다.
				// 추워=춥+다
			}
							  //이전음절의 받침없고
			else if (fvl_prev[2] == 1 && fvl[0] == 13 &&
							  // 현재음절이 와/워 인 경우
				(fvl[1] == 14 || fvl[1] == 21)
				/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
			{				  // ㅂ 불규칙
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				// 여기서부터는 str_last 도 설정해주어햔다.
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf("ㅂ불규칙:%s %s \n",str_first,str_last);
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (len - i == 2 && fvl_prev[2] == 1
				&& !strncmp ((char*)str + i, "이", 2))
			{
				//ㅂ 불규칙 : 가까이, 와 같이 ㅂ 탈락현상
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "이", 2);
				str_last[2] = 0;
				if (is_belong_to_yongon_dic (str_first))
					save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (fvl_prev[2] == 1 && !strncmp ((char*)str + i, "우", 2))
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
		// 이후부터는 어미가 변하는 경우
		if (eomi_flag)
		{
			//printf("f=%d v=%d l=%d \n",fvl_prev[0],fvl_prev[1],fvl_prev[2]);
			// 종성이 ㄴ/ㄹ/ㅁ/ㅂ 인 경우 종성을 제거한 부분+ 종성+어미 로 분리(p.27)
			// 예를 들어 가다 에 ㅂ니다 가 합쳐딘 경우 갑니다 가 되는데,
			// 이 경우 어미가 니다 가 아닌 ㅂ니다 로 분리가 되고 어간이 가 가 되어야 함
			if (fvl[2] == 5 || fvl[2] == 9 || fvl[2] == 17 || fvl[2] == 19)
			{
				//printf("ma: ㄴ,ㄹ,ㅁ,ㅂ 의 종성 \n");
							  // 받침을 제거
				convert_3_to_ks (fvl[0], fvl[1], 1, tmp);
							  // 이전음절이 받침이 없고, 우로 끝나는 경우
				if (fvl_prev[2] == 1 &&
					!strncmp ((char*)tmp, "우", 2))
				{			  // 아름다운, 아름다울 과 같은 ㅂ 불규칙
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
					//printf("ㅂ불규칙:%s %s \n",str_first,str_last);
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
				else if (check_belong_to_info_table_yongon (tmp, len_word))
				{
					// 받침을 제거한 부분의 음절이 용언의 끝음절로 올 수 있나체크
					//printf(" 용언+ ㄴ,ㄹ,ㅁ,ㅂ \n");
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
				{			  // ㄴ
					convert_3_to_ks (fvl[0], fvl[1], 9, tmp);
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "ㄴ", 2);
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
					str_last[len - i] = 0;
					if (is_belong_to_eomi_table (str_last))
					{
						if (is_belong_to_yongon_dic (str_first))
							save_result_yongon (nth, str_first, NULL, str_last);
					}
				}

			}
			else if (!strncmp ((char*)str + i, "해", 2))
			{
				strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) "하", 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
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
			{				  // 7=ㅓ
				strncpy ((char*)str_last, (char*) "어", 2);
				if (len > i - 2)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
				{
					if (i > 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					if (!strncmp ((char*)str + i, "러", 2))
					{		  // '르' 불규칙과 '러' 불규칙
						if (fvl_prev[2] == 9)
						{
							convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
							strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
							strncpy ((char*)str_first + i, (char*) "르", 2);
							str_first[i + 2] = 0;
							if (is_belong_to_yongon_dic (str_first))
							{
								save_result_yongon (nth, str_first, NULL,
									str_last);
							}
							  // 앞에서 변형된 어간을 원상으로
							strncpy ((char*)str_first, (char*) str, i);
						}
						else if (!strncmp ((char*)str_first + i - 2, "르", 2))
						{
							str_first[i] = 0;
							if (is_belong_to_yongon_dic (str_first))
							{
								save_result_yongon (nth, str_first, NULL,
									str_last);
							}
						}
					}
					if (!strncmp ((char*)str + i, "퍼", 2))
					{
							  // p.67
						strncpy ((char*)str_first + i, (char*) "푸", 2);
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
				//       3=ㅏ ,  4=ㅐ , 10= ㅔ
				//  예) 나가 = 나가+아, 포개서 =포개+어서 , 세 = 세어
				status_del = 0;
				if (check_belong_to_info_table (str + i, F_END_1))
				{			  // ㅏ 로 끝나는 경우
					strncpy ((char*)str_last, (char*) "아", 2);
					status_del = 1;
				}
							  // ㅐ
				else if (check_belong_to_info_table (str + i, F_END_4) ||
					check_belong_to_info_table (str + i, F_END_5))
				{			  // ㅔ
					strncpy ((char*)str_last, (char*) "어", 2);
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
			{				  // ㅕ 로 끝나는 경우
				convert_3_to_ks (fvl[0], 29, 1, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) tmp, 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
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
				//                  14=ㅘ,  21=ㅝ,  11=ㅕ
							  // 13=ㅗ
				if (fvl[1] == 14)
					convert_3_to_ks (fvl[0], 13, 1, tmp);
							  // 20= ㅜ
				else if (fvl[1] == 21)
					convert_3_to_ks (fvl[0], 20, 1, tmp);
				//printf("tmp=%s \n",tmp);
				if (check_belong_to_info_table (tmp, F_END_3))
				{			  // ㅜ+ ㅓ 혹은 ㅗ+ㅏ
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "어", 2);
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
				if (strncmp ((char*)str + i, "꾀", 2) && strncmp ((char*)str + i, "뇌", 2) &&
					strncmp ((char*)str + i, "외", 2))
				{			  // p.67
					strncpy ((char*)str_first, (char*) str, i);
					convert_3_to_ks (fvl[0], 18, 1, tmp);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "어", 2);
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

	// 먼저 음절중 용언에만 올 수 있는 음절이 들어있는지 체크 해서
	// 용언에만 올수 있으면 체언+조사의 가능성은 없음
	status_yongon_only = check_yongon_only (str, len);
	if (status_yongon_only)
		josa_flag = 0;
	else
		josa_flag = 1;
	eomi_flag = 1;

	len_total = token_to_johabs (str, len, token_johab);

	// 첫음절에 어미부분이 포함되어 있을 가능성이 있는경우와 그렇지 않은 경우구분
	// 예1) 갈 사람 -> 가 +ㄹ, 간다고 -> 가 + ㄴ다고
	// ㄴ,ㄹ 이 어미이지만 첫음절 용언의 받침이 없으므로
	// 첫음절에 포함 된다.
	// 예2) 서 버렸다. ->서 + 어 용언으 모음이 받침없는 ㅏ, ㅓ, ㅐ, ㅔ 로 끝나는
	// 경우 어미 '아' 나 어 가 생략되므로 첫음절에 어미가 있다고 생각할 수 있다.
	// 예3) 써 = 쓰 + 어 ㅡ 축약현상
	// 예4) 펴 = 피 + 어, 꿔 = 꾸 + 어 , 와 = 오 + 아 등등
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
		// 여기서부터 용언분리과정
		// 먼저 첫음절까지 체크할 필요가 없는 경우
		if (i >= 2 && eomi_flag)
		{
			// 먼저 어미가 변하지 않는 경우
			if (check_belong_to_info_table (str + i, F_EOMI1))
			{
				// 현재음절이 어미의 첫번째 올수있는지
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				//printf(" str_last=%s \n",str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					// 현재음절이후 문자열이 어미사전(테이블)에 등록되어 있는지
					if (fvl_prev[2] == 22)
					{		  //전음절에  ㅆ 받침이 있는 경우
						j = 0;
						while (i / 2 - 2 - j > 0
							&& token_johab[i / 2 - 2 - j][2] == 22)
						{
							j++;
						}
							  // ipos 는 ㅆ 이 들어간 음절의 시작점
						ipos = i - 2 - 2 * j;
						if (strncmp ((char*)str + ipos, "있", 2))
						{	  // 있 은 선어말 어미가 아님
							  // 선어말어미 분리
							pos_pf = treat_pf (str, str_first, str_middle, token_johab[ipos / 2], ipos, i);
							save_result_yongon (nth, str_first, str_middle,
								str_last);
						}
						else
						{	  // '있' 의 경우
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
					else if (i >= 4 && !strncmp ((char*)str + i - 2, "시", 2))
					{		  //ㅆ 없이 직접시가 오는경우
						if (i > 4 && !strncmp ((char*)str + i - 4, "으", 2))
						{
							//시 앞에 으 는 선어말어미
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
							if (!strncmp ((char*)str + i - 2, "이", 2))
								save_result_yongon_plus_yi (nth, str_first,
									str_middle, str_last);
						}
					}
					else
					{		  // ㅆ 이나 '(으)시' 가 없는 경우, 어미는 변하지 않는 경우
						strncpy ((char*)str_first, (char*) str, i);
						str_first[i] = 0;
						save_result_yongon (nth, str_first, NULL, str_last);
						if (!strncmp ((char*)str + i - 2, "이", 2))
							save_result_yongon_plus_yi (nth, str_first, NULL,
								str_last);
					}
				}
				// 이후는 선어말 어미가 없는 경우
				// 여 불규칙, 너라 불규칙, 거라 불규칙
			}
			else
			if ((!strncmp ((char*)str + i - 2, "하", 2)
				&& !strncmp ((char*)str + i, "여", 2))
				||
				((!strncmp ((char*)str + i - 2, "가", 2)
				|| !strncmp ((char*)str + i - 2, "오", 2)
				|| !strncmp ((char*)str + i - 2, "있", 2)
				|| !strncmp ((char*)str + i - 2, "자", 2))
				&& !strncmp ((char*)str + i, "거라", 4))
				|| (!strncmp ((char*)str + i - 2, "오", 2)
				&& !strncmp ((char*)str + i, "너라", 4)))
			{
				strncpy ((char*)str_first, (char*) str, i - 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) str + i, len - i);
				str_last[len - i] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
				// 어미가 변하는 경우
				// 어간도 변하는 경우
				// ㅂ 불규칙의 경우도 둘째음절까지만 살펴도 된다.
				// 추워=춥+다
			}
							  //이전음절의 받침없고
			else if (fvl_prev[2] == 1 && fvl[0] == 13 &&
							  // 현재음절이 와/워 인 경우
				(fvl[1] == 14 || fvl[1] == 21)
				/* &&  check_belong_to_info_table(str+i-2,F_BUL_B) */ )
			{				  // ㅂ 불규칙
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				// 여기서부터는 str_last 도 설정해주어햔다.
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf("ㅂ불규칙:%s %s \n",str_first,str_last);
				save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (len - i == 2 && fvl_prev[2] == 1
				&& !strncmp ((char*)str + i, "이", 2))
			{
				//ㅂ 불규칙 : 가까이, 와 같이 ㅂ 탈락현상
				convert_3_to_ks (fvl_prev[0], fvl_prev[1], 19, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i - 2);
				strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
				str_first[i] = 0;
				strncpy ((char*)str_last, (char*) "이", 2);
				str_last[2] = 0;
				save_result_yongon (nth, str_first, NULL, str_last);
			}
			else if (fvl_prev[2] == 1 && !strncmp ((char*)str + i, "우", 2))
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
		// 이후부터는 어미가 변하는 경우

		if (eomi_flag)
		{
			//printf("f=%d v=%d l=%d \n",fvl_prev[0],fvl_prev[1],fvl_prev[2]);
			// 종성이 ㄴ/ㄹ/ㅁ/ㅂ 인 경우 종성을 제거한 부분+ 종성+어미 로 분리(p.27)
			// 예를 들어 가다 에 ㅂ니다 가 합쳐딘 경우 갑니다 가 되는데,
			// 이 경우 어미가 니다 가 아닌 ㅂ니다 로 분리가 되고 어간이 가 가 되어야 함
			if (fvl[2] == 5 || fvl[2] == 9 || fvl[2] == 17 || fvl[2] == 19)
			{
				//printf("ma: ㄴ,ㄹ,ㅁ,ㅂ 의 종성 \n");
							  // 받침을 제거
				convert_3_to_ks (fvl[0], fvl[1], 1, tmp);
							  // 이전음절이 받침이 없고, 우로 끝나는 경우
				if (fvl_prev[2] == 1 &&
					!strncmp ((char*)tmp, "우", 2))
				{			  // 아름다운, 아름다울 과 같은 ㅂ 불규칙
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
					//printf("ㅂ불규칙:%s %s \n",str_first,str_last);
					if (is_belong_to_eomi_table (str_last))
					{
						save_result_yongon (nth, str_first, NULL, str_last);
					}
				}
				else if (check_belong_to_info_table_yongon (tmp, len_word))
				{
					// 받침을 제거한 부분의 음절이 용언의 끝음절로 올 수 있나체크
					//printf(" 용언+ ㄴ,ㄹ,ㅁ,ㅂ \n");
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
						if (strncpy ((char*)tmp, (char*) "이", 2))
							save_result_yongon_plus_yi (nth, str_first, NULL,
								str_last);
					}
				}

			}
			else if (!strncmp ((char*)str + i, "해", 2))
			{
				strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) "하", 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
				strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				//printf( "%s  %s \n",str_first,str_last);
				if (is_belong_to_eomi_table (str_last))
				{
					save_result_yongon (nth, str_first, NULL, str_last);
				}
			}
			else if (fvl[2] == 1 && fvl[1] == 7)
			{				  // 7=ㅓ
				strncpy ((char*)str_last, (char*) "어", 2);
				if (len > i - 2)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
				{
					if (i > 2)
						strncpy ((char*)str_first, (char*) str, i - 2);
					if (!strncmp ((char*)str + i, "러", 2))
					{		  // '르' 불규칙과 '러' 불규칙
						if (fvl_prev[2] == 9)
						{
							convert_3_to_ks (fvl_prev[0], fvl_prev[1], 1, tmp);
							strncpy ((char*)str_first + i - 2, (char*) tmp, 2);
							strncpy ((char*)str_first + i, (char*) "르", 2);
							str_first[i + 2] = 0;
							save_result_yongon (nth, str_first, NULL, str_last);
							  // 앞에서 변형된 어간을 원상으로
							strncpy ((char*)str_first, (char*) str, i);
						}
						else if (!strncmp ((char*)str_first + i - 2, "르", 2))
						{
							str_first[i] = 0;
							save_result_yongon (nth, str_first, NULL, str_last);
						}
					}
					if (!strncmp ((char*)str + i, "퍼", 2))
					{
							  // p.67
						strncpy ((char*)str_first + i, (char*) "푸", 2);
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
				//       3=ㅏ ,  4=ㅐ , 10= ㅔ
				//  예) 나가 = 나가+아, 포개서 =포개+어서 , 세 = 세어
				status_del = 0;
				if (check_belong_to_info_table (str + i, F_END_1))
				{			  // ㅏ 로 끝나는 경우
					strncpy ((char*)str_last, (char*) "아", 2);
					status_del = 1;
				}
							  // ㅐ
				else if (check_belong_to_info_table (str + i, F_END_4) ||
					check_belong_to_info_table (str + i, F_END_5))
				{			  // ㅔ
					strncpy ((char*)str_last, (char*) "어", 2);
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
			{				  // ㅕ 로 끝나는 경우
				convert_3_to_ks (fvl[0], 29, 1, tmp);
				if (i >= 2)
					strncpy ((char*)str_first, (char*) str, i);
				strncpy ((char*)str_first + i, (char*) tmp, 2);
				str_first[i + 2] = 0;
				strncpy ((char*)str_last, (char*) "어", 2);
				if (len - i - 2 >= 0)
					strncpy ((char*)str_last + 2, (char*) str + i + 2, len - i - 2);
				str_last[len - i] = 0;
				if (is_belong_to_eomi_table (str_last))
					save_result_yongon (nth, str_first, NULL, str_last);

			}
			else if (fvl[2] == 1 &&
				(fvl[1] == 14 || fvl[1] == 21 || fvl[1] == 11))
			{
				//                  14=ㅘ,  21=ㅝ,  11=ㅕ
							  // 13=ㅗ
				if (fvl[1] == 14)
					convert_3_to_ks (fvl[0], 13, 1, tmp);
							  // 20= ㅜ
				else if (fvl[1] == 21)
					convert_3_to_ks (fvl[0], 20, 1, tmp);
				if (check_belong_to_info_table (tmp, F_END_3))
				{			  // ㅜ+ ㅓ 혹은 ㅗ+ㅏ
					strncpy ((char*)str_first, (char*) str, i);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "어", 2);
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
				if (strncmp ((char*)str + i, "꾀", 2) && strncmp ((char*)str + i, "뇌", 2) &&
					strncmp ((char*)str + i, "외", 2))
				{			  // p.67
					strncpy ((char*)str_first, (char*) str, i);
					convert_3_to_ks (fvl[0], 18, 1, tmp);
					strncpy ((char*)str_first + i, (char*) tmp, 2);
					str_first[i + 2] = 0;
					strncpy ((char*)str_last, (char*) "어", 2);
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
