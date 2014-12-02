#define NO_AUX_YONGON 37
char *aux_yongon[]=
{
	"계시",					  // 0 아,어,고
	"있",					  //
	"못하",					  // 1 지, 다
	"법하",					  // 2 ㄹ
	"보",					  // 3 다, 다가, 아, 어
	"생기",					  // 4 게
	"굴",
	"성싶",					  // 5 ㄴ, 은, 을 , ㄹ, 는
	"성하",
	"시프",					  // 6 고,은가,ㄴ가,는가,를까,을까,지
	"싶",
	"아니하",				  // 7 지
	"않",
	"하",					  // 8 기만,으면,면,기도,게,기나,아,어,으려고,려고,고자,아야
	"터지",					  // 9 아,어
	"가",
	"내",
	"놓",
	"달",
	"대",
	"두",
	"드리",
	"마지하니하",
	"버릇하",
	"버리",
	"빠지",
	"오",
	"주",
	"죽겠",
	"지",
	"치우",
	"싶어하",				  // 10 고
	"만들",					  // 11 게,도록
	"나",					  // 12 아,어,고
	"말",					  // 13 지,고,고야
	"바치",					  // 14 어,어다
	"셈치"					  // 15 으,은,는
};
int aux_yongon_index[]=
{
	0, 0, 1, 2, 3,  4, 4, 5, 5, 6,
	6, 7, 7, 8, 9,  9, 9, 9, 9, 9,
	9, 9, 9, 9, 9,  9, 9, 9, 9, 9,
	9,10,11,12,13, 14,15
};
int no_aux_yongon_type[]={3,2,1,4,1,5,7,1,13,2,1,2,3,3,2,3};
char *aux_yongon_prev_type0[]=
{
	"아","어","고"
};
char *aux_yongon_prev_type1[]=
{
	"지","다"
};
char *aux_yongon_prev_type2[]=
{
	"ㄹ"
};
char *aux_yongon_prev_type3[]=
{
	"다","다가","아","어"
};
char *aux_yongon_prev_type4[]=
{
	"게"
};
char *aux_yongon_prev_type5[]=
{
	"ㄴ","은","ㄹ","을","는"
};
char *aux_yongon_prev_type6[]=
{
	"고","은가","ㄴ가","는가","를까","을까","지"
};
char *aux_yongon_prev_type7[]=
{
	"지"
};
char *aux_yongon_prev_type8[]=
{
	"기만","으면","면","기도","게","기나","아","어","으려고","려고","고자","아야",
	"기로"
};
char *aux_yongon_prev_type9[]=
{
	"아","어"
};
char *aux_yongon_prev_type10[]=
{
	"고"
};
char *aux_yongon_prev_type11[]=
{
	"게","도록"
};
char *aux_yongon_prev_type12[]=
{
	"아","어","고"
};
char *aux_yongon_prev_type13[]=
{
	"지","고","고야"
};
char *aux_yongon_prev_type14[]=
{
	"어","어다"
};
char *aux_yongon_prev_type15[]=
{
	"으","은","는"
};

//unsigned char **aux_yongon_prev[16];
char **aux_yongon_prev[16];
