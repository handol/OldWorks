
#define USE_REAL_ACE

#ifdef USE_REAL_ACE
#include "ace/Log_Msg.h"
#include "ace/streams.h"
#else
#include "Log_Msg.h" // NOT Real ACE. My lib
#endif

#include <stdio.h>
#include <ctype.h>

#include "CLog.h"
#include "MemSplit.h"
#include "StrUtil.h"

#include "HDict.h"
#include "HGram.h"
#include "HStem.h"
#include "HProc.h"

int HProc::dict(char *query, StrStream &resStr)
{
	HwordType *w =0;
	w = HDict::search(query);
	if (w){
		HDict::printWord_stream(resStr, w);
	}
	else {
		resStr.add("Word NOT found\n");
	}
	return 0;
	
}

int HProc::stem(char *query, StrStream &resStr)
{
	char	hword[32];
	int	hwlen = 0;
	HStem stemmer;
	
	hwlen = HanChar::getHangulOnly(hword, sizeof(hword)-1, query );
	if (hwlen==0) return 0;
	stemmer.stem(hword, hwlen );
	stemmer.printStem_stream(resStr);
	stemmer.printBestStem_stream(resStr);
	return 0;
}

/**

*/
int HProc::test_stemmer(char *fname, StrStream &resStr)
{

	FILE *fp;
	char buf[256];
	
	int	cnt=0;
	MemSplit	line(50, MAXLEN_HWORD);
	char	hword[32];
	int	hwlen = 0;

	if ( (fp=fopen(fname, "r")) == NULL) {
		ACE_DEBUG((LM_DEBUG,"Read Failed: %s\n", fname)); 
		return -1;
	}

	//StrUtil::path_merge(logname, adhome, "log/stem.log")
	CLog mylog(".", "stem.log");
	while (fgets(buf, sizeof(buf)-1, fp) != NULL) {
		int	ch = StrUtil::first_nonspace(buf);		
		if (ch==0 || ch=='#') continue;
		if (ch=='$') break;
		line.reuse();
		line.split(buf);
		if (line.size()==0) continue;
		//line.print();

		
		for(int i=0; i<line.size(); i++) {
			HStem stemmer;

			hwlen = HanChar::getHangulOnly(hword, sizeof(hword)-1, line.str(i) );
			if (hwlen==0) continue;
			stemmer.stem(hword, hwlen );
			stemmer.printStem_stream(resStr);

			if (resStr.len() > 5)
			mylog.LogPRN2("%s\n%s\n", hword, resStr.str() );
			else
			mylog.LogPRN2("%s\nFail\n\n", hword);
				
		}
			
		cnt++;

	}

	ACE_DEBUG((LM_DEBUG, "FILE %s :  count=%d\n", fname, cnt ));
	
	fclose(fp);
	return 0;
}

