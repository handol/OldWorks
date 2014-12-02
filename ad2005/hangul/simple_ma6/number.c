/* by S. C. Lee
license GPL
*/
#include <stdio.h>
#include <string.h>

char str_number[][3]=
{
	"¿µ", "ÀÏ","ÀÌ","»ï","»ç","¿À",
	"À°","Ä¥","ÆÈ","±¸"
};

char str_unit[][3]={ "½Ê","¹é","Ãµ","¸¸","¾ï" };

char str_number_result[100];

extern int len_basic;
int convert_number_to_str(char *str_number_tmp, int no_of_char)
{
	int i;
	int unit,unit4;
	int index;
	int status;
	index=0;

	for(i=0;i<no_of_char;i++)
	{
		unit4=(no_of_char-i-1)/4;
		unit=(no_of_char-i-1) - unit4*4;
		status=0;
		if(str_number_tmp[i]!='0')
		{
			if( str_number_tmp[i] !='1' || ( str_number_tmp[i]=='1' && unit==0 ) )
			{
				strncpy(&str_number_result[index],str_number[str_number_tmp[i]-48 ],2 );
				index+=2;
			}
			status=1;
		}
		if(unit4 !=0 && unit==0)
		{
			strncpy(&str_number_result[index],str_unit[unit4+2],2);
			index+=2;
		}
		else if( status==1 && unit !=0)
		{
			strncpy(&str_number_result[index],str_unit[unit-1],2);
			index+=2;
		}
	}
	return index;
}
