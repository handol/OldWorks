#include "PhoneTrace.h"
#include "Util.h"
#include <iostream>

using namespace std;

/**
@briesf
@return false 목록에 포함되지 않는 경우.
*/

bool PhoneTrace::isTrace(char *MDN)
{
	intMDN_t mdn = Util::phonenum2int(MDN);
	return isTrace(mdn);
}

bool PhoneTrace::isTrace(intMDN_t MDN)
{
	PhoneTraceList::const_iterator p;
	
	p = traceList.find(MDN);
	if (p != traceList.end()) {
		return true;
	}
	else {
		return false;
	}
}

PhoneTraceInfo *PhoneTrace::search(intMDN_t MDN)
{
	PhoneTraceList::const_iterator p;
	p = traceList.find(MDN);
	if (p != traceList.end()) {		
		return p->second;
	}
	else {
		return NULL;
	}
}

/*

int PhoneTrace::add(char *MDN)
{
	intMDN_t mdn = Util::phonenum2int(MDN);
	return add(mdn);
}

int PhoneTrace::add(intMDN_t MDN)
{
	size_t	lastsize = traceList.size();
	//traceList[MDN] = 1;
	//traceList.insert(make_pair(MDN, 1));
	if (traceList.size() > lastsize) 
		return 1;
	else
		return 0;
}
*/

int PhoneTrace::add(PhoneTraceInfo *info)
{
	PhoneTraceInfo *toadd = new PhoneTraceInfo();
	size_t	lastsize = traceList.size();

	memcpy(toadd, info, sizeof(PhoneTraceInfo));
	
	traceList[toadd->mdn] = toadd;
	//traceList.insert(make_pair(MDN, 1));
	if (traceList.size() > lastsize) 
		return 1;
	else
		return 0;
}

void	PhoneTrace::clear()
{
	PhoneTraceList::const_iterator p;
	for(p=traceList.begin(); p != traceList.end(); p++)
		if (p->second)
			delete p->second;
	traceList.clear();
}
void PhoneTrace::prn(StrStream_t  &out)
{
	out.sprintf("== Phone Trace List ==\n");
	PhoneTraceList::const_iterator p2;
	for (p2 = traceList.begin(); p2 != traceList.end(); p2++) {
		out.sprintf("%d\n", p2->first);
		#ifdef DEBUG
		cout << "key= " << p2->first << endl;
		cout << "Val= " << p2->second << endl;
		#endif
	}
}


#ifdef TEST_MAIN

int main()
{
	PhoneTrace trace;

	trace.add("0114300258");
	trace.add("0117781220");
	trace.add("023334444");

	
	trace.isTrace("0114300258");
}
#endif
