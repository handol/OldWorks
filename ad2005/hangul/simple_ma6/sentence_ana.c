/* by s.c. Lee */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ma.h"
#include "aux_noun.h"
#include "aux_yongon.h"
enum
{
	dummy3, A_CHAON, A_YONGON, A_BUSA, A_KWANHYUNGSA,
	A_CHAON_YI,A_GAMTANSA
};
extern int initial_end_verb_incomp[];
extern STR_RESULT_MA result_ma[100];
int status_analyzed[100];
int depth_and_kind[100][2];

unsigned char token[100][100];
extern int sp;
unsigned char screen_line[20][400];
char *str_type[]=
//unsigned char *str_type[]=
{
	"(no)","(��)","(��)","(��)","(��)","(��)"
};

void print_final_result(int);
int check_unanalyzed_word(int index);
int check_compatability_with_sentence_ana(int,int);
int analyze_modification(int index,int kind,int depth);
int analyze_yongon(int i,int depth);

int is_belong_to_verb_incomp_dic(int index)
{
	FILE *fp;
	L_RESULT_MA *p_list_result;
	int len;
	char fname[20];
	int result;
	unsigned char *str;
	//printf(" is_belong_to_verb_incomp_dic : \n");
	p_list_result=result_ma[index].p_list_result;
	str=p_list_result->yongon.eogan;
	len=strlen((char*)str);
							  // xx�� �� ��� ��� �ϴ� Ÿ�����
	if( !strncmp((char*)str+len-2,"��",2)) return 1;
	if( len/2 > 6) return 0;  // �� ���ڴ�  ����
	sprintf(fname,"verb_incomp_%d.dic",len/2);
	fp=fopen(fname,"r");
	result=b_search(VERB_INCOMP,fp,0,
		initial_end_verb_incomp[len/2-1]*len,str,len);
	fclose(fp);
	//printf("result=%d \n",result);
	return result;
}


int is_belong_to_josa_subj_obj( char *str, int kind)
{
	if(kind==3) return 1;
	if(kind==1)
	{
		if( !strcmp(str,"��") || !strcmp(str,"��") ||
			!strcmp(str,"��") || !strcmp(str,"��") )
			return 1;
	}
	else if(kind==2)
	{
		if( !strncmp(str,"��", 2) || !strncmp(str,"��", 2) )
			return 1;
	}else
	printf(" is_belong_to_josa_subj_obj: something wrion! \n");
	return 0;
}


int is_belong_to_aux_noun_type( char *str,int type)
{
	int i,len;

	len=strlen(str);
	str=str+len-2;
	//printf(" is_belong_to_aux_noun_type: str=%s, type=%d no=%d \n",
	//      str, type, no_aux_noun_type[type]);
	for(i=0;i<no_aux_noun_type[type];i++)
	{
		if( !strncmp((char*)aux_noun_prev[type][i],str,2) )
			return 1;
	}
	return 0;
}


int check_conjunction_with_aux_noun(int index, int type, int kind,int depth)
{
	int index_non_ana;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		//printf(" check_conjunction_with_aux_noun: eogan=%s eomi=%s \n",
		//          p_list_result->yongon.eogan,p_list_result->yongon.eomi);
		if( p_list_result->type==S_KWANHYUNGSA )
		{
			if( type==0 | type == 6)
			{
				status_analyzed[index+1]=A_CHAON;
				depth_and_kind[index+1][0]=depth;
				depth_and_kind[index+1][1]=kind;
				status_analyzed[index]=A_KWANHYUNGSA;
				depth_and_kind[index][0]=depth+1;
							  // 4=���ľ�
				depth_and_kind[index][1]=4;
				if( (index_non_ana=check_unanalyzed_word(index+2) ) >=0)
				{
					analyze_modification(index_non_ana,kind+1,depth+1);
				}
				return 1;
			}else
			return 0;
		}else if( p_list_result->type==EOGAN_PEOMI_EOMI ||
			p_list_result->type==EOGAN_EOMI ||
			p_list_result->type==CHAON_YI_EOMI ||
			p_list_result->type==CHAON_YI_PEOMI_EOMI
			)
		{
			if( is_belong_to_aux_noun_type((char*)p_list_result->yongon.eomi,type) )
			{
				printf(" ������+ �������� \n");
				status_analyzed[index+1]=A_CHAON;
				depth_and_kind[index+1][0]=depth;
				depth_and_kind[index+1][1]=kind;
				if( (index_non_ana=check_unanalyzed_word(index+2) ) >= 0)
				{
					analyze_modification(index_non_ana,kind+1,depth+1);
				}
				if(index>0)	  // �ּ��� �ι�° ���� ����
					analyze_yongon(index,depth+1);

				return 1;
			}
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_aux_noun_and_ana( int index,unsigned char *str,int kind, int depth)
{
	int i;
	int type;
	for(i=0;i<NO_AUX_NOUN;i++)
	{
		if( !strcmp((char*)str,aux_noun[i]) )
		{
			type=aux_noun_index[i];
			if( check_conjunction_with_aux_noun(index-1,type,kind, depth))
			{
				//printf(" check_aux_noun_and_ana: index-1=%d \n",index-1);
				return index;  // +1 ��
			} else
			return 0;
		}
	}
	return 0;
}


int is_subj_obj(int index, int kind,int depth)
{
	// �м������ϸ� 0 �� �ƴ� ���� ��ȯ , ��������� ���� �Ϲ�ü���� ���
	L_RESULT_MA *p_list_result;
	unsigned char *p_str;
	int index_start;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==CHAON_JOSA )
		{
			if( is_belong_to_josa_subj_obj( (char*)p_list_result->chaon.josa, kind ) )
			{
				if(index>0)	  // ��ּ��� �ι�° ����
					if( (index_start=
					check_aux_noun_and_ana(index, p_list_result->chaon.chaon,
					kind,depth) ) >0 )
				{
					printf(" index_start=%d \n");
							  // �ҿ��� ������ ���
					return -index_start;
				}
							  // �Ϲ� ü���� ���
				return index+1;
			}
		}
		else if( p_list_result->type==S_CHAON )
		{
			p_str=p_list_result->single;
			if( kind==1)	  // subject
			{
				if (!strcmp((char*) p_str, "��" ) )
				{
					if( (index_start=check_aux_noun_and_ana( index, p_str,
						kind,depth)) >0 )
					{
						//printf(" �ҿ��� ���� index_start=%d\n", index_start);
						return -index_start;
					}
				}
			}				  // object
			else if( kind==2)
			{

			}
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_unanalyzed_word(int index)
{
	if( status_analyzed[index] !=0 ) return -1;
	while( status_analyzed[index] ==0 ) index++;
	return index-1;
}


int find_and_analyze_subj_obj(int index,int kind, int depth)
{
	int i,j;
	int type;
	for(i=index-1;i>=0;i--)
	{
		if(status_analyzed[i] >0 ) return -1;
		if( (type=is_subj_obj(i,kind,depth))!=0 )
		{
			if(type> 0)
			{
				status_analyzed[i]=A_CHAON;
				if( (j=check_unanalyzed_word(i+1)) >=0 )
				{
					analyze_modification(j,kind+1,depth+1);
				}
				depth_and_kind[i][0]=depth;
							  // 1- �־� 2-������ 3-����
				depth_and_kind[i][1]=kind;
			}				  //��������� �̹� ����� ����
			else
			{
				//printf(" find_and_analyze_sibj_obj: from %d to %d \n",abs(type), i);
			}
			return abs(type);
		}
	}
	return -1;
}


int check_yongon(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==EOGAN_PEOMI_EOMI ||
			p_list_result->type==EOGAN_EOMI ||
			p_list_result->type==CHAON_YI_EOMI ||
			p_list_result->type==CHAON_YI_PEOMI_EOMI
			)
		{
			return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int is_belong_to_aux_yongon_type( unsigned char *str,int type)
{
	int i,len;
	//printf(" is_belong_to_aux_yongon_type : str=%s type=%d \n",str,type);
	for(i=0;i<no_aux_yongon_type[type];i++)
	{
		if( !strcmp((char*)aux_yongon_prev[type][i],(char*)str) )
			return 1;
	}
	return 0;
}


int check_conjunction_with_aux_yongon(int index, int type)
{
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==EOGAN_PEOMI_EOMI ||
			p_list_result->type==EOGAN_EOMI )
		{
			//printf("check_conjunction_with_aux_yongon : \n");
			if( is_belong_to_aux_yongon_type(p_list_result->yongon.eomi,type) )
			{
				return 1;
			}
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int is_aux_yongon( int index)
{
	int i;
	int type;
	for(i=0;i<NO_AUX_YONGON;i++)
	{
		if( !strcmp((char*)result_ma[index].p_list_result->yongon.eogan,(char*)aux_yongon[i]) )
		{
			//printf(" is_aux_yongon : \n");
			type=aux_yongon_index[i];
			if( check_conjunction_with_aux_yongon(index-1,type))
			{
				return 1;
			}  else
			return 0;
		}
	}
	return 0;
}


int check_is_chaon_plus_yi(int index, int depth)
{
	int i;
	int len;
	int index_start;
	unsigned char str[10];
	L_RESULT_MA *p_list_result;

	//printf(" check_is_chaon_plus_yi :\n ");
	p_list_result=result_ma[index].p_list_result;
	//  if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==CHAON_YI_EOMI ||
			p_list_result->type==CHAON_YI_PEOMI_EOMI )
		{
			len=strlen((char*)p_list_result->yongon.eogan);
			strncpy((char*)str,(char*)p_list_result->yongon.eogan,len-2);
			str[len-2]=0;
			if( (index_start=
				check_aux_noun_and_ana(index, str,3, depth)) >0 )
			{
				//printf(" index_start=%d str=\n", index_start,str);
							  // overwrite
				status_analyzed[index]=A_CHAON_YI;
							  // �ҿ��� ������ ���
				return index_start;
			}
			//printf(" �Ϲ�ü��+�� \n");
			status_analyzed[index]=A_CHAON_YI;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=3;
			return index+1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_busa(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==S_BUSA)
		{
			return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_kwanhyungsa(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==S_KWANHYUNGSA)
		{
			return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_kwanhyung_yongon(int index)
{
	L_RESULT_MA *p_list_result;
	//unsigned char *p_str;
	char *p_str;
	int len;

	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		p_str=(char*)p_list_result->yongon.eomi;
		len=strlen(p_str);
		p_str=p_str+len-2;
		if(!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) )
			return 1;
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_chaon_possesive(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==CHAON_JOSA)
		{
			if( !strcmp((char*)p_list_result->chaon.josa,"��") )
				return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_chaon_plus_josa(int index)
{
	L_RESULT_MA *p_list_result;
	unsigned char *p_str;
	int index_start;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==CHAON_JOSA )
		{
			return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_single_chaon(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==S_CHAON)
		{
			return 1;
		}
		p_list_result=p_list_result->next;
	}
	return 0;
}


int analyze_modification(int index,int kind,int depth)
{
	int type_of_next;
	int type;
	if( depth==0 ) printf( " analyze_modification: something wrong !\n");
	if( status_analyzed[index] != 0) return 0;
	type_of_next=status_analyzed[index+1];
	if( type_of_next == A_YONGON)
	{
		if( check_busa(index) )
		{
			status_analyzed[index]=A_BUSA;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
			if(index >0 && status_analyzed[index-1]==0 )
				analyze_modification(index-1,kind,depth+1);
		}
		else if( check_yongon(index) )
		{
			analyze_yongon(index,depth);
		}
		else if( check_chaon_plus_josa(index) )
		{
			status_analyzed[index]=A_CHAON;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
			if(index >0 && status_analyzed[index-1]==0 )
				analyze_modification(index-1,kind,depth+1);
		}
	}
	else if( type_of_next ==A_BUSA)
	{
		if( check_busa(index) )
		{
			status_analyzed[index]=A_BUSA;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
			if(index >0 && status_analyzed[index-1]==0 )
				analyze_modification(index-1,kind,depth+1);
		}
		else if( check_yongon(index) )
		{
			analyze_yongon(index,depth);
		}
	}
	else if( type_of_next == A_CHAON || type_of_next == A_CHAON_YI)
	{
		if( check_kwanhyungsa(index) )
		{
			status_analyzed[index]=A_KWANHYUNGSA;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
		}
		else if( check_yongon(index) )
		{
			if( check_kwanhyung_yongon(index) )
			{
				analyze_yongon(index,depth+1);
			}				  // ����� �������� �ƴѰ�� õ������ �� �� ����.
			else if( index> 0)
			{
				analyze_yongon(index,depth+1);
			}
		}
		else if( check_chaon_possesive(index) )
		{
			//printf(" possesive \n");
			status_analyzed[index]=A_CHAON;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
			if(index >0 && status_analyzed[index-1]==0)
				analyze_modification(index-1,kind,depth);
		}					  // �� ��� ���ո���
		else if( check_single_chaon(index) )
		{
			status_analyzed[index]=A_CHAON;
			depth_and_kind[index][0]=depth-1;
			depth_and_kind[index][1]=4;
			if(index > 0 && status_analyzed[index-1]==0 )
				analyze_modification(index-1, kind,depth);
		}					  // �� ��� �λ�� ��ü������ ����
		else if( check_busa(index) )
		{
			status_analyzed[index]=A_BUSA;
			depth_and_kind[index][0]=depth;
			depth_and_kind[index][1]=4;
			if(index >0 && status_analyzed[index-1]==0 )
				analyze_modification(index-1,kind,depth+1);
		}
	}
}


int analyze_yongon(int i,int depth)
{
	int index;
	int index_yongon;
	int index_object;
	int index_subject;
	int type_yongon;
	unsigned char str_ana[50];
	if(i>0)
	{
		index_yongon=-1;
		index_object=-1;
		index_subject=-1;
		if( ( index= check_is_chaon_plus_yi(i,depth)) !=0 )
		{
			//printf(" check_is_chaon_plus_yi is ok \n");

			index_yongon=index;
		}
		else if( is_aux_yongon(i) )
		{
			status_analyzed[i]=A_YONGON;
			depth_and_kind[i][0]=depth;
							  // 3=�����
			depth_and_kind[i][1]=3;
							  // ��𿡼� ����� ���� ����
			index_yongon=analyze_yongon(i-1,depth);
			return index_yongon;
		}					  // Ÿ������ ���
		else if( is_belong_to_verb_incomp_dic(i) )
		{
			//printf(" Ÿ���� \n");
			index_yongon=i+1;
			status_analyzed[i]=A_YONGON;
			depth_and_kind[i][0]=depth;
			depth_and_kind[i][1]=3;
			index_object=find_and_analyze_subj_obj(i,2,depth);
			if(index_object==0)
				index_object=-1;
		}
		else
		{
			//printf(" �ڵ��� \n");
			index_yongon=i+1;
			status_analyzed[i]=A_YONGON;
			depth_and_kind[i][0]=depth;
			depth_and_kind[i][1]=3;
		}
		if(index_object>=0)
		{
			index_subject=find_and_analyze_subj_obj(index_object-1,1,depth);
		}
		else if(index_yongon >=0 )
		{
			index_subject=find_and_analyze_subj_obj(index_yongon-1,1,depth);
		}
		else
		{
			index_subject=find_and_analyze_subj_obj(i,1,depth);
		}
	}
	else if( check_kwanhyung_yongon(0) )
	{
		// ������ ù �������� ������� �ܿ��� ����� ���� ����
		return 0;
	}						  // �׷��� �������θ� ������ ������ ����
	else if(depth==0)
	{
		status_analyzed[0]=A_YONGON;
		depth_and_kind[0][0]=0;
		depth_and_kind[0][1]=3;
		return 0;
	}

	/*
	printf(" index obj=%d yongon=%d subj=%d depth=%d\n",index_object,index_yongon,
						  index_subject,depth);
	*/
	if(index_subject !=-1)
	{
		return index_subject;
	}else if( index_object !=-1 )
	return index_object;
	else if( index_yongon !=-1)
		return index_yongon;
	else
		return i+1;
}


void init_table()
{
	aux_noun_prev[0]=aux_noun_prev_type0;
	aux_noun_prev[1]=aux_noun_prev_type1;
	aux_noun_prev[2]=aux_noun_prev_type2;
	aux_noun_prev[3]=aux_noun_prev_type3;
	aux_noun_prev[4]=aux_noun_prev_type4;
	aux_noun_prev[5]=aux_noun_prev_type5;
	aux_noun_prev[6]=aux_noun_prev_type6;

	aux_yongon_prev[0]=aux_yongon_prev_type0;
	aux_yongon_prev[1]=aux_yongon_prev_type1;
	aux_yongon_prev[2]=aux_yongon_prev_type2;
	aux_yongon_prev[3]=aux_yongon_prev_type3;
	aux_yongon_prev[4]=aux_yongon_prev_type4;
	aux_yongon_prev[5]=aux_yongon_prev_type5;
	aux_yongon_prev[6]=aux_yongon_prev_type6;
	aux_yongon_prev[7]=aux_yongon_prev_type7;
	aux_yongon_prev[8]=aux_yongon_prev_type8;
	aux_yongon_prev[9]=aux_yongon_prev_type9;
	aux_yongon_prev[10]=aux_yongon_prev_type10;
	aux_yongon_prev[11]=aux_yongon_prev_type11;
	aux_yongon_prev[12]=aux_yongon_prev_type12;
	aux_yongon_prev[13]=aux_yongon_prev_type13;
	aux_yongon_prev[14]=aux_yongon_prev_type14;
	aux_yongon_prev[15]=aux_yongon_prev_type15;

}


int check_josa_chaon_connection(int index)
{
	L_RESULT_MA *p_list_result;
	char *p_str;
	int len;
	return 1;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		p_str=(char*)p_list_result->chaon.josa;
		len=strlen(p_str);
		p_str=p_str+len-2;
		if(!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) ||
			!strncmp(p_str,"��",2) || !strncmp(p_str,"��",2) )
			return 1;
		p_list_result=p_list_result->next;
	}
	return 0;
}


int check_josa_yongon_connection(int index)
{
	return 0;
}


void free_p_list_result(int index)
{
	L_RESULT_MA *p_list_result;

	p_list_result=result_ma[index].p_list_result;
	free(p_list_result);
	result_ma[index].no_of_result=0;
}


void merge_composite_noun(unsigned char *str,int len,int index)
{
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return ;

	printf("str=%s \n",str);
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==EOGAN_PEOMI_EOMI ||
			p_list_result->type==EOGAN_EOMI ||
			p_list_result->type==CHAON_YI_EOMI ||
			p_list_result->type==CHAON_YI_PEOMI_EOMI )
		{
			strcpy((char*)str+len,(char*)p_list_result->yongon.eogan);
			strcpy((char*)p_list_result->yongon.eogan,(char*)str);
		}
		else if( p_list_result->type==CHAON_JOSA )
		{
			strcpy((char*)str+len,(char*)p_list_result->chaon.chaon);
			strcpy((char*)p_list_result->chaon.chaon,(char*)str);
		}

		p_list_result=p_list_result->next;
	}
}


int check_composite_noun(unsigned char *str, int len,int nth)
{
	int i;
	unsigned char str_first[20];
	unsigned char str_last[20];
	if(len<6) return 0;
	for(i=2;i<len;i+=2)
	{
		strncpy((char*)str_first,(char*)str,i);
		str_first[i]=0;
		if( is_belong_to_chaon_dic(str_first) )
		{
			token_to_ma(str+i,nth,1);
			if( result_ma[nth].no_of_result > 0)
			{
				merge_composite_noun(str_first,i,nth);
				return 1;
			}
		}
	}
	return 0;
}


int guess_ma(int index)
{
	unsigned char *str;
	unsigned char str_added[50];
	int len;

	str=token[index];
	len=strlen((char*)str);
	if(len==0 ) return 0;
							  // �ٶ���� üũ
	if( index >0 && result_ma[index-1].no_of_result==0 )
	{
		sprintf((char*)str_added,"%s%s",token[index-1],str);
		token_to_ma(str_added,index,2);
		if( result_ma[index].no_of_result > 0 )
		{
			strcpy((char*)token[index],(char*)str_added);
			return 1;
		}
	}
	if( check_composite_noun(str,len,index) )
	{
		return 1;
	}
	guess_token_to_ma(str,index,2);
	if( result_ma[index].no_of_result==0)
		save_result_single_word(index,token[index],S_CHAON);
	return 1;
}


// �м����� ���� ���
// ���� ��� ����� ���� ��� ������� ������� �м�
// �־ ���� ��� �־�� ������ ���� �м�
// �־� ���� �м��� �������� ���� ��� ���..
int analyze_words_left(int no_of_words)
{
	int i;
	int depth;
	unsigned char str[20];
	int len;

	for(i=no_of_words-1;i>=0;i--)
	{
		if( status_analyzed[i]==0 )
		{
			if( i< no_of_words-1 )
			{
				depth=depth_and_kind[i+1][0];
				printf(" index=%d \n", i);
				if( result_ma[i].no_of_result==0 )
				{
				} else
				analyze_modification(i,0,depth+1);
			}
		}
	}
	return 0;
}


int get_max_depth(int no_of_words)
{
	int i;
	int max;
	max=0;
	for(i=0;i<no_of_words;i++)
	{
		if( depth_and_kind[i][0] > max ) max=depth_and_kind[i][0];
	}
	return max;
}


// ������ �������� ���Ӹ��� ���°� ���°��
char *table_special[]=
{
	"�ž�","�ſ���","�Ŵ�","�״�","�ų�","�Ŷ��"
};
char *table_eogan_special[]=
{
	"����","����","����","����","����","����"
};
char *table_eomi_special[]=
{
	"��","����","��","��","��","���"
};

int check_special_case(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	for(i=0;i<6;i++)
	{
		if(!strcmp((char*)token[index],(char*)table_special[i]) )
		{

			result_ma[index].no_of_result=1;
			p_list_result=result_ma[index].p_list_result;
			p_list_result->type=EOGAN_EOMI;
			strncpy((char*)p_list_result->yongon.eogan,(char*)table_eogan_special[i],4);
			p_list_result->yongon.eogan[4]=0;
			strcpy((char*)p_list_result->yongon.eomi,(char*)table_eomi_special[i]);
			return 1;
		}
	}
	return 0;
}


int check_gamtansa(int index)
{
	int i;
	L_RESULT_MA *p_list_result;
	p_list_result=result_ma[index].p_list_result;
	if( result_ma[index].no_of_result==0) return 0;
	while( p_list_result!=NULL)
	{
		if( p_list_result->type==S_GAMTANSA )
			return 1;
		p_list_result=p_list_result->next;
	}
	return 0;
}


void sentence_ana(int no_of_words)
{
	int i,j,k;
	int index;
	int max_depth;
	int type_yongon;
	int status;
	int pos_word[100];
	int depth,kind;
	int pos,len;
	static int status_init_table=1;
	for(i=0;i<no_of_words;i++)
	{
		if(result_ma[i].no_of_result==0)
			guess_ma(i);
	}
	print_result(no_of_words);
	if(status_init_table==1)
	{
		init_table();
		status_init_table=0;
	}
	for(i=0;i<20;i++)
	{
		for(j=0;j<400;j++)
		{
			screen_line[i][j]=' ';
		}
	}
	for(i=0;i<no_of_words;i++)
	{
		depth_and_kind[i][0]=-1;
	}
	for(i=0;i<no_of_words;i++)
		status_analyzed[i]=0;
	if( check_gamtansa(0) )
	{
		printf(" ��ź�� \n");
		status_analyzed[0]=A_GAMTANSA;
		depth_and_kind[0][0]=0;
		depth_and_kind[0][1]=5;
	}
	i=no_of_words-1;
	if( check_yongon(i) )
	{
		status=analyze_yongon(i,0);
		if(status>1)
			analyze_modification(status-2,1,1);
	}
	else if( check_special_case(i) )
	{
		status=analyze_yongon(i,0);
		if(status>1)
			analyze_modification(status-2,1,1);
	}else if(status<0) printf(" ������ �ƴ� ��\n");
	analyze_words_left(no_of_words);
	/* 
	for(i=0;i<no_of_words;i++)
	  printf( "status_analyzed[%d]=%d ",i,status_analyzed[i]);
	printf("\n");
	*/

	for(i=0;i<no_of_words;i++)
		pos_word[i]=0;
	for(i=0;i<no_of_words;i++)
	{
		if(i>0)
			pos_word[i]=pos_word[i-1]+strlen((char*) token[i-1] )+2;
		depth=depth_and_kind[i][0];
		kind=depth_and_kind[i][1];
		//printf("depth=%d pos=%d \n",depth, pos_word[i]);
		if( depth>=0)
		{
			strncpy((char*) screen_line[2*depth]+pos_word[i], (char*)str_type[kind],4);
			strncpy((char*) screen_line[2*depth+1]+pos_word[i], (char*)token[i], strlen((char*)token[i]));
		}
	}
	max_depth=2+2*get_max_depth(no_of_words);
	for(i=0;i<max_depth;i++)
	{
		screen_line[i][pos_word[no_of_words-1]+strlen((char*)token[no_of_words-1])]=0;
		printf("%s\n",screen_line[i]);
	}
	print_final_result(no_of_words);
}


int check_compatability_with_sentence_ana( int type_ma, int nth)
{
	if( status_analyzed[nth]==A_CHAON_YI)
	{
		if( type_ma==CHAON_YI_PEOMI_EOMI || type_ma==CHAON_YI_EOMI) return 1;
	}
	else if( status_analyzed[nth]==A_CHAON)
	{
		if( type_ma==CHAON_JOSA || type_ma==S_CHAON ) return 1;
	}
	else if( status_analyzed[nth]==A_YONGON)
	{
		//printf(" yongon type=%d %d %d\n", type_ma,EOGAN_PEOMI_EOMI, EOGAN_EOMI);
		if( type_ma==EOGAN_PEOMI_EOMI || type_ma==EOGAN_EOMI ) return 1;
	}
	else if( status_analyzed[nth]==A_BUSA)
	{
		if( type_ma==S_BUSA ) return 1;
	}
	else if( status_analyzed[nth]==A_KWANHYUNGSA)
	{
		if( type_ma==S_KWANHYUNGSA) return 1;
	}
	else if( status_analyzed[nth]==A_GAMTANSA)
	{
		if( type_ma==S_GAMTANSA) return 1;
	}else if( status_analyzed[nth]==0) return 1;
	return 0;

}


void print_final_result_word(int nth_word)
{
	int type;
	L_RESULT_MA *l_result_ma;

	l_result_ma=result_ma[nth_word].p_list_result;
	if( l_result_ma==NULL) return;
	while( l_result_ma !=NULL)
	{
		type=l_result_ma->type;
		if( check_compatability_with_sentence_ana( type, nth_word) )
		{
			if( type==EOGAN_PEOMI_EOMI || type==CHAON_YI_PEOMI_EOMI)
			{
				//printf(" eogan_peomi_eomi \n");
				print_result_yongon(l_result_ma->yongon.eogan,
					l_result_ma->yongon.peomi,
					l_result_ma->yongon.eomi);
			}
			else if( type == EOGAN_EOMI || type==CHAON_YI_EOMI )
			{
				//printf(" eogan_eomi \n");
				print_result_yongon(l_result_ma->yongon.eogan,NULL,
					l_result_ma->yongon.eomi);
			}
			else if( type== CHAON_JOSA )
			{
				print_result_chaon(l_result_ma->chaon.chaon, l_result_ma->chaon.josa);
			}
			else if( type==S_BUSA )
			{
				printf("(%s/�λ�) \n",l_result_ma->single);
			}
			else if(type==S_KWANHYUNGSA )
			{
				printf("(%s/������) \n",l_result_ma->single);
			}
			else if(type==S_CHAON)
			{
				printf("(%s/ü��) \n",l_result_ma->single);
			}
			else if(type==S_GAMTANSA)
			{
				printf("(%s/��ź��) \n",l_result_ma->single);
			}
		}
		l_result_ma=l_result_ma->next;
	}
}


void print_final_result(int no_of_word)
{
	int i;
	for(i=0;i<no_of_word;i++)
		print_final_result_word(i);
}