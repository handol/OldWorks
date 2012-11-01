#ifndef BASICDEF_H
#define BASICDEF_H

#ifndef WIN32
#	if defined(WINDOWS) || defined(_WIN32)
#		define	WIN32
#	endif
#endif

// 한글 처리시에 isspace 함수를 사용하게 되면 문제가 발생한다.
#ifndef ISSPACE
#define	ISSPACE(X)	((X)==' ' || (X)=='\t' || (X)=='\n' || (X)=='\r')
#endif

#ifndef ISDIGIT
#define	ISDIGIT(X) ((X) >= '0' && (X) <= '9')
#endif


#ifndef TRUE
#define TRUE 1
#endif


#ifndef FALSE
#define FALSE 0
#endif


#ifndef MAX
#define MAX(A, B) ((A)>(B))?(A):(B)
#endif

#ifndef MIN
#define MIN(A, B) ((A)<(B))?(A):(B)
#endif

#ifndef BETWEEN
#define BETWEEN(X, A, B) ((A)<=(X) && (X) <= (B))
#endif


#define	NOUSE_ARG(X)	while(0) X

#define	Int_at_Addr(X) ( *((int*)(X)) )
//#define PERCENT(A,B) (int) (((float)(A)/(float)(B))*(float)100)
#define PERCENT(A,B) (int) (100*(A)/(B))

#define IN_RANGE(X, A, B) ((A)<=(X) && (X) <= (B))
#define OUT_RANGE(X, A, B) ((A)>(X) || (X) > (B))


#define STRCPY(DEST, SRC, LEN)	{strncpy((DEST), (SRC), (LEN)); (DEST)[LEN] = 0;}
#define STRNCPY(DEST, SRC, LEN)	{strncpy((DEST), (SRC), (LEN)); (DEST)[LEN] = 0;}
#define	ALIGN_HALF_K(Len)	(((Len >> 9) + 1) << 9)

typedef unsigned int uint4;
typedef unsigned short uint2;
typedef unsigned char byte;



#define stringize(s) #s
#define XSTR(s) stringize(s)
#if !defined NDEBUG
#include <stdlib.h>
void abort (void);
# if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
# define ASSERT(a) \
do { \
if (0 == (a)) { \
fprintf(stderr, \
"Assertion failed: %s, " \
"%s(), %d at \'%s\'\n", \
__FILE__, \
__func__, \
__LINE__, \
XSTR(a)); \
abort(); \
} \
} while (0)
# else
# define ASSERT(a) \
do { \
if (0 == (a)) { \
fprintf(stderr, \
"Assertion failed: %s, " \
"%d at \'%s\'\n", \
__FILE__, \
__LINE__, \
XSTR(a)); \
abort(); \
} \
} while (0)
# endif
#else
# define ASSERT(a) (void)0
#endif




#endif
