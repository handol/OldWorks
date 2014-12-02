#include <stdio.h>

#include <string>
#include <list>

using namespace std;

main()
{
	list<int> a;
	list<string> b;
	a.push_back(10);
	b.push_back("aaa");
	b.push_back("bbb");

	/*
	for(int i=0; i<b.size(); i++) {
		printf("%s\n", b.
	}
	*/

	list<string>::iterator iter;
	
	for (iter=b.begin(); iter != b.end(); iter++)
	{
		printf("%s\n", (*iter).c_str() );
	}
}
