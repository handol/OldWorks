/**
HAM  �ý��ۿ��� ����ϴ� header ȭ�Ϸκ��� �ѱ� ������ ����� ���Ѵ�.
*/

#include <stdio.h>
#include "hdics.h"
#include "sfx-v.h"
#include "sfx-n.h"
#include "xomi.h"
#include "pomi.h"

#include "joemlist.h"
#include "eomilist.h"
#include "josalist.h"

void prn_pomidic()
{
	int	i;
	printf("#=== pomidic \n");
	for(i=0; i<N_POMI; i++) {
		printf("%s\n", pomidic[1][i]);
	}
	printf("#===\n\n");
}

void prn_xomidic()
{
	int	i;
	printf("#=== xomidic \n");
	for(i=0; i<N_XOMI; i++) {
		printf("%s\n", xomidic[1][i]);
	}
	printf("#===\n\n");
}

void prn_nsfdic()
{
	int	i;
	printf("#=== nsfdic \n");
	for(i=0; i<N_NSFX; i++) {
		printf("%s\n", nsfdic[1][i]);
	}
	printf("#===\n\n");
}



void prn_vsfdic()
{
	int	i;
	printf("#=== vsfdic \n");
	for(i=0; i<N_VSFX; i++) {
		printf("%s\n", vsfdic[1][i]);
	}
	printf("#===\n\n");
}

void	prn_kor_fix()
{
	prn_pomidic();
	prn_xomidic();
	prn_nsfdic();
	prn_vsfdic();
}

void	prn_eomi()
{
	int	i;
	printf("## ��� ���\n");
	for(i=0; i<sizeof(EOMILIST)/sizeof(HAM_JOEMLIST); i++)
		printf("%-3d %s\n", i+1, EOMILIST[i].joem);
	printf("## %d\n", i);
}

void	prn_josa()
{
	int	i;
	printf("## ����  ���\n");
	for(i=0; i<sizeof(JOSALIST)/sizeof(HAM_JOEMLIST); i++)
		printf("%-3d %s\n", i+1, JOSALIST[i].joem);
	printf("## %d\n", i);
}

int
main (int argc, char **argv)
{
	if (argc > 1 && argv[1][0]=='j') {
		prn_josa();
	}

	if (argc > 1 && argv[1][0]=='e') {
		prn_eomi();
	}

	if (argc > 1 && argv[1][0]=='a') {
		prn_kor_fix();
	}

}

