#define NO_AUX_NOUN 52
//unsigned char *aux_noun[]=
char *aux_noun[]=
{
	"��",					  // 0-8 ���� ������, ��, ��, (��)��, (��)�� �ڿ�
	"����",
	"��",
	"��",
	"��",
	"��",
	"����",
	"��",
	"ġ",
	"�븩",					  //9 9-20 ����, ��, (��)�� (��)�� �ڿ�
	"��",
	"���",
	"��",
	"��",
	"��",
	"��ŭ",
	"��",
	"��",
	"��",
	"��",
	"��",
	"��",					  // 21-23 ���� ��, (��)�� �ڿ�
	"����",
	"ü",
	"��",					  // 24-28 ���� �� �ڿ�
	"��",
	"�ٶ�",
	"��",
	"ǰ",
	"��",					  // 29-31 ���� (��)�� �ڿ�
	"��",
	"ä",
	"��",
	"����",					  // 33-48 ���� (��)�� �ڿ�
	"����",
	"��",
	"����",
	"��",
	"����",
	"����",
	"��",
	"��",
	"��",
	"��",					  //39
	"��",
	"¦",
	"����",
	"��",
	"��",
	"��",					  // 49-52 ���� ������, �� �ڿ�
	"����",
	"��"
};
int aux_noun_index[]=
{
	0,
	0,0,0,0,0,  0,0,0,1,1,
	1,1,1,1,1,  1,1,1,1,1,
	2,2,2,3,3,  3,3,3,4,4,
	4,5,5,5,5,  5,5,5,5,5,
	5,5,5,5,5,  5,5,5,6,6,
	6
};
char *aux_noun_prev_type0[]=
//unsigned char *aux_noun_prev_type0[]=
{
	"��","��","��","��","��","��"
};

char *aux_noun_prev_type1[]=
//unsigned char *aux_noun_prev_type1[]=
{
	"��","��","��","��","��"
};
char *aux_noun_prev_type2[]=
{
	"��","��","��"
};
char *aux_noun_prev_type3[]=
{
	"��"
};
char *aux_noun_prev_type4[]=
{
	"��","��"
};
char *aux_noun_prev_type5[]=
{
	"��","��"
};
char *aux_noun_prev_type6[]=
{
	"��"
};
int no_aux_noun_type[]={6,5,3,1,2,2,1};
//unsigned char **aux_noun_prev[7];
char **aux_noun_prev[7];

int  aux_noun_josa_flag[]=
{
	1,						  // "��",
	1,						  //"����",
	1,						  //"��",
	1,						  //"��",
	1,						  //"��",
	1,						  //"��",
	3,						  //"����",
	1,						  //"��",
	0,						  //"ġ",
	3,						  //"�븩",
	7,						  //"���",
	1,						  //"��",
	4,						  //"��",
	7,						  //"��",
	7,						  //"��ŭ",
	1,						  //"��",
	4,						  //"��",
	0,						  //"��",
	4,						  //"��",
	3,						  //"��",
	6,						  //"��",
	3,						  //"����",
	4,						  //"ü",
	1,						  //"��",
	0,						  //"��",
	6,						  //"�ٶ�",
	6,						  //"��",
	0,						  //"ǰ",
	0,						  //"��",
	2,						  //"��",
	5,						  //"ä",
	3,						  //"��",
	3,						  //"����",
	2,						  //"����",
	1,						  //"��",
	3,						  //"����",
	2,						  //"��",
	0,						  //"����",
	1,						  //"����",
	7,						  //"��",
	7,						  //"��",
	3,						  //"��",
	7,						  //"��",
	6,						  //"��",
	2,						  //"��",
	6,						  //"¦",
	1,						  //"����",
	3,						  //"��",
	2,						  //"��",
	6,						  //"��",
	1,						  //"����",
	3,						  //"��"
};