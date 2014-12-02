/* by S. C. Lee
license GPL
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern unsigned char str_number[][2];
extern unsigned char str_unit[][2];

int status_hangul_hanja;

extern void ttma(unsigned char *str,int len);
extern int convert_number_to_str(char *str_number_tmp, int no_of_char);
extern int get_hangul_code(int start, int end, int hanja);

int len_basic;
main(int argc ,char *argv[])
{
	FILE *fp;
	int i,j;
	char fname[20];
	unsigned char *str_sampa[3];
	unsigned char str[1000];
	int mode;
	int index;
	int c;
	int status_number;
	unsigned char str_number_tmp[10];
	unsigned char str_number_result[100];
	int len;
	int hanja_code,hanja_first, hangul_code;

	mode=0;
	len_basic=250;
	for(i=1;i<argc;i++)
	{
		if(!strcmp(argv[i],"-f"))
		{
			strcpy((char*)fname,argv[i+1]);
			mode=1;
		}
		else if(!strcmp(argv[i],"-t"))
		{
			strcpy((char*)str,argv[i+1]);
			mode=2;
		}
		else if(!strcmp(argv[i],"-s"))
		{
			strcpy((char*)str,argv[i+1]);
			len_basic=atoi((char*)str);
		}
	}
	if(mode==0)
	{
		strcpy((char*)str,"나는 학교에 갑니다.");
	}
	if(mode!=1)
	{
		ttma(str,strlen((char*)str));
	}
	else
	{
		fp=fopen(fname,"r");
		if (fp==0) {
			printf("read failed: %s\n", fname);
			exit(0);
		}
		index=0;
		status_number=0;
		status_hangul_hanja=0;
		while( (c=fgetc(fp) )!=EOF)
		{
							  // 숫자
			if( c>=48 && c <=57)
			{
				str_number_tmp[status_number]=c;
				status_number++;
			}
			else
			{
				if(status_number !=0)
				{
					str_number_tmp[status_number]=0;
					len=convert_number_to_str((char*)str_number_tmp,status_number);
					str_number_result[len]=0;
					status_number=0;
					strncpy((char*)&str[index],(char*)str_number_result,len);
					index+=len;
				}
				if( status_hangul_hanja==0 && c>128)
				{
					if(  c >= 0xb0 && c <= 0xc8 )
					{
						str[index++]=c;
						status_hangul_hanja=1;
					}
					else if ( c >=0xca && c <=0xfd )
					{
						hanja_first=c;
						status_hangul_hanja=2;
					}
				}
				else if( status_hangul_hanja==1 && c > 128 )
				{
					str[index++]=c;
					status_hangul_hanja=0;
				}
				else if( status_hangul_hanja==2 && c> 128 )
				{
					hanja_code=hanja_first*256+c;
					hangul_code=get_hangul_code(0,512,hanja_code);
					str[index++]=hangul_code/256;
					str[index++]=hangul_code%256;
					status_hangul_hanja=0;
				}
				else
				{
					if( index !=0 &&
						(c=='.' || c=='?' || c=='!')  )
					{
						str[index++]=c;
						ttma(str,index);
						index=0;
					}
					else if( index > 50 && c==',')
					{
						str[index++]=c;
						ttma(str,index);
						index=0;
					}
					else if(c==' ')
					{
						str[index++]=c;
					}
					else if(c>=48 && c<=57)
					{
						status_number++;
						strncpy((char*)&str[index],(char*)str_number[c-48],2);
						index+=2;
					}
				}
			}

		}
		fclose(fp);
	}
}
