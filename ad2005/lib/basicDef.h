#ifndef BASICDEF_H
#define BASICDEF_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef WIN32
#	if defined(WINDOWS) || defined(_WIN32)
#		define	WIN32
#	endif
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

#define	STRNCPY(DEST, SRC, N)	{ strncpy((DEST), (SRC), (N)); (DEST)[N] = 0; }

#define	NOUSE_ARG(X)	while(0) X

#define	Int_at_Addr(X) ( *((int*)(X)) )
//#define PERCENT(A,B) (int) (((float)(A)/(float)(B))*(float)100)
#define PERCENT(A,B) (int) (100*(A)/(B))

#define IN_RANGE(X, A, B) ((A)<=(X) && (X) <= (B))
#define OUT_RANGE(X, A, B) ((A)>(X) || (X) > (B))


typedef unsigned int uint4;
typedef unsigned short uint2;
typedef unsigned char byte;

#define	SBJ_LEN (64) // docs 의 subject 길이 
#define URL_LEN	(100)
#define HOST_LEN	(31)
#define PATH_LEN	(100)

#define MAX_COLS (24)
#define STR_LEN (100)

#define WORD_LEN (48)

#define	HTML_SIZE	(512*1024)
#define	TEXT_SIZE	(512*1024)
#define	TRIM_SIZE	(512*1024)

#define	MAX_PAGE_TRV	(2048)

#define	MAX_NEW_WC	(9000) // 새 word_combi 수 
#define	MAX_NEW_WORD (5000) // 새 idiom (합성 어 ) 수 
#define	MAX_NEW_IDIOM (1000) // 새 idiom (합성 어 ) 수 

#define	ALIGN_HALF_K(Len)	(((Len >> 9) + 1) << 9)

#define	PRINTF(X)	printf X
#endif
