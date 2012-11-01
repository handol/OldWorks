/**
@file StrSplit.h

@brief String 분리 유틸리티

주어진 line (string) 을 공백 기준으로 나누어, string array 에 저장한다.
string array의 메모리는 동적으로 할당한다.

MemSplit을 단순화하고, 변수명 리팩토링. handol@gmail.com 2006/08/02

MemSplit	line(12, 64);

while (fgets(buf, 1024, fp) != NULL) {
	line.split(buf);
	if (line.numFlds()==0) continue;

	somefunc( line.fldVal(0), line.fldLen(0) );  // 첫번째 필드 처리

	if (line.has("zeta"))
		printf("This line has zeta !");
}
*/

#ifndef STRSPLIT_H
#define STRSPLIT_H


#define	MAX_SPLIT_FLDS (32)  /**< 분리된 필드들의 최대 개수 */

#ifndef ISSPACE
/* 한글 처리 시에 isspace () 사용하면 문제가 되는 것으로 보임. */
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

class StrSplit
{
	public:
		StrSplit() { init(); }
		StrSplit(int numOfFlds, int lengOfFld) {
			init();
			alloc(numOfFlds, lengOfFld);
		}


		~StrSplit() {
			clear();
		}

		void    init() {
			maxNumFlds = maxLengFlds = numSplit = 0;
		}

		void alloc(int numOfFlds, int lengOfFld);
		void clear();

		int split(char *src); // 공백으로 분리.
		int split(char *src, char ch); // 주어진 문자로 분리.
		int splitWords(char *src); // alphabet 으로 구성된 단어만 추출.

		int has(char *str); // 분리된 필드들 중에  주어진 문자열과 동일한 것이 있는지 검사.


		char *fldVal(int index) { return fldVals[index]; } // 필드 값. 문자열.
		int fldLen(int index) { return fldLengs[index]; } // 필드 길이
		int numFlds() { return numSplit; } // 분리된  필드의 개수.

		char **argv() { return fldVals; } // 필드를 문자열의 array 처럼 사용하고자 할 때
		int argc() { return numSplit; }

		void print(char *msg=NULL); // 모든 필드를 출력.

	private:
		int maxNumFlds;
		int maxLengFlds;
		int numSplit;							  /* split 된 결과 string의 개수 */
		char *fldVals[MAX_SPLIT_FLDS];
		int fldLengs[MAX_SPLIT_FLDS];
		char    *memory;

};
#endif
