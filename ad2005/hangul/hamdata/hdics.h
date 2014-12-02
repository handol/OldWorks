/*
	File Name: hdics.h
	Description:
		Dic. file names used in HAM.
	Written by: Kang, Seung-Shik	04/11/1997
*/

/*-------------------------------------------------------------------
	Dictionary file names used in HAM.
		HANGUL.DIC : Hangul word dictionary for noun, verb, etc.
		HANGUL.USR : user-defined dictionary
		HANGUL.TRM : terminology and name dictionary
		HANGUL.RMA : Ready-Made Analysis dictionary
-------------------------------------------------------------------*/
#define HAN_DIC		"hangul.dic"
#define USER_DIC	"ham-usr.dic"
#define TERM_DIC	"ham-trm.dic"
#define RMA_DIC		"ham-rma.dic"

/* stopword dictionary only for automatic indexing */
#define STOP_DIC	"stopword.dic"

/* Josa/Eomi dic. is loaded from 'josaeomi.dic' */
#define FWD_DIC		"josaeomi.dic"	/* not used any more */
#define FWD_DI0		"josaeomi.di0"

/* backup & temporary files for dic. management */
#define USER_DI0	"ham-usr.us0"
#define TERM_DI0	"ham-trm.tr0"
#define STOP_DI0	"stopword.di0"

#define HANDIC_LOG	"_hangul.log"
#define TEMP_FILE	"_hangul.$$$"

/* 6 dic. files at 'hangul.dic' */
#define HDICSIZE	302574L
#define HDICFILES	6

#define KDIC_NV1	0
#define KDIC_N02	1
#define KDIC_N03	2
#define KDIC_N48	3
#define KDIC_VJX	4
#define KDIC_AID	5

/*-------------------------------------------------------------------
	Base index of dicinfo[] for verbs et al.(adv, det, excl)
-------------------------------------------------------------------*/
#define VPOSBASE	12
#define APOSBASE	93	/* 91+2 : add 2 for 'JTKV' */

/*
	number of noun suffixes and max. bytes
*/
#define N_NSFX	40
#define NSFXSIZE	7

/*
	number of verb suffixes and max. bytes
*/
#define N_VSFX	15
#define VSFXSIZE	5

/*
	number of p-Eomi's and max. bytes
*/
#define N_POMI	68
#define POMISIZE	11

/*
	number of Eomi's of preceding x-verb and max. bytes
*/
#define N_XOMI	12
#define XOMISIZE	3

/*
	number of words & max. bytes in 'hangul.usr'.
*/
#define NO_POS		100
#define MAXUSERWORDS	50000
#define MAXUSERBYTES	300000L

/*
	number of words & max. bytes in 'hangul.rma'.
*/
#define MAXRMAWORDS	5000
#define MAXRMABYTES	40000

/*
	number of words & max. bytes in 'hangul.top'.
*/
#define MAXSTOPWORDS	10000
#define MAXSTOPBYTES	60000

/*-------------------------------------------------------------------
	Dic. file definition for spelling correction
-------------------------------------------------------------------*/
/* reverse Hangul dic. for spelling correction */
#define SPL_DIC		"hspell.dic"

/* 8 dic. files at 'hspell.dic' */
#define SDICSIZE	563342L
#define SDICFILES	8

#define SDIC_EJO	0
#define SDIC_N01	1
#define SDIC_N02	2
#define SDIC_N03	3
#define SDIC_N48	4
#define SDIC_V01	5
#define SDIC_VJX	6
#define SDIC_AID	7

#if 0 // @dahee
/*-------------------------------------------------------------------
	Dic. files for Korean morphological analysis
-------------------------------------------------------------------*/
typedef struct {
	HAM_SLONG base[HDICFILES];	/* file base for 6 KDICs */
	HAM_SLONG bas2[SDICFILES];	/* file base for 8 SDICs */
#ifdef HDIC_FILE
	FILE_PTR hdic;	/* Hangul lexical dictionaries           */
	FILE_PTR tdic;	/* Korean name and terminologies dic.    */
	FILE_PTR udic;	/* user-defined dictionary               */
#else
	HAM_UCHAR hdic[HDICSIZE];/* Hangul lexical dictionaries  */
/*	HAM_PUCHAR tdic; Korean name and terminologies dic.    */
/*	HAM_PUCHAR udic; user-defined dictionary               */
	HAM_PUCHAR sdic; /* reverse dic. for spelling correction  */
#endif
/*	FILE_PTR rdic;	 Ready-Made Analysis dictionary        */
} HAM_DICS, HAM_FAR * HAM_PDICS;
/*-----------------  This is the end of hdics.h ------------------*/
#endif
