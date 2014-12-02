#ifndef EDICT_H
#define EDICT_H

#include "wordType.h"
#include "simpleHash.h"
#include "StrStream.h"

//#include "Thread_Mutex.h"

#define	DICT_SIZE	(1024*100)
class EDict {
	public:
		/* load: 
			text file 방식, MSSQL DB 방식, MySql DB 방식 .
			inherit 방식으로 클래스 구성할까 ?
		*/
		static EDict * getInstance(int hashsize=0);
		
		static int count()
		{
			return hashTab.count ();
		}
		static void print()
		{
			hashTab.print();
		}

		static wordType * search(char *key);
		
		static wordType * insert(char *key);

		static wordType * insert(wordType *newone);
		
		
		static 	int remove(wordType *delinfo);

		static	wordType *checkConjugate(char *word, int wlen, byte *conj_PS);
		static	wordType *checkDerivative(char *word, int wlen, byte *conj_PS);
		static	wordType *stemWord(char *org_w, int wlen, byte *conj_PS);
		
		static 	int	prepare(char *home=0);
		static 	int	loadWords_FILE(char *fname);
		static 	int	loadWords();
		static	int	markBasicWords(char *fname);
		static 	int	loadIdioms_FILE(char *fname);
		static 	int	loadIdioms();
		static 	int	connectWordIdiom();
		static 	void	printWord(wordType *w);
		static	void	printWord(wordType *w, StrStream &stream);

		static	void	printIdiom(idiomType *idiom);		
		static	void	printIdiom(idiomType *idiom,  StrStream &stream);

		static	int sortIdioms_in_word();
		static	int cmpIdioms_in_word(const void *Aptr, const void *Bptr);

		
		static	int cmpWordList(const void *Aptr, const void *Bptr);
		static	int sortWordList();
		static	wordType *searchWord(int target_id);
		
		static	int cmpIdiomList(const void *Aptr, const void *Bptr);
		static	int sortIdiomList();
		static	idiomType *searchIdiom(int target_id);

		
		static	int	testIdiom(char *buf,  StrStream &stream);
		static	int	stemNwords(wordType	*wordInfos[], int maxnum, char *buf, char *firstWord=NULL);
		static	idiomType *searchIdiom(wordType **words, int num_words);
		static	idiomType *searchIdiom_partial(wordType **words, int num_words);
		static	int	matchIdiom(wordType **words, int num_words, wordType **idiom_words, int idiom_w_cnt);
		
		
	private:
		
		EDict ()
		{
		}

		static EDict* _instance;
		
		static simpleHash<wordType> hashTab;

		#ifdef MUTEX
		static ACE_Thread_Mutex hashmutex_;
		#endif

		static	simpleAlloc mymem;

		static	wordType	**wordArray;
		static	int			wordNums;
		static	idiomType	**idiomArray;
		static	int			idiomNums;
		
		
};

#endif

