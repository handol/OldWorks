#ifndef EXAM_H
#define EXAM_H
#include "wordType.h"
#include "simpleAlloc.h"
#include "StrStream.h"

class Exam
{
public:
	static	int	cmpEx(exType *exA, exType *exB);
	static	int	addEx(wordType *word, exType *newex);
	static	int	addEx(idiomType *word, exType *newex);
	static	void	printEx(exType *ex);
	static	void	printWordEx(wordType *word);
	static	int	loadWordEx(char *homepath, simpleAlloc &mem);
	static	void	printExStc(exType *ex, StrStream &stream);
	static	void	printWordExStc(wordType *word, StrStream &stream);
	static	void	printIdiomExStc(idiomType *word, StrStream &stream);
	static	int	loadWordEx_DB(simpleAlloc &mem);
	static	int	loadIdiomEx_DB(simpleAlloc &mem);

	static	int comp_exampl_doc_id(const void *A, const void *B);
	static	int sort_exampl_doc_id(exType *arr, int size);
	static	int comp_exampl_and_many(const void *A, const void *B);
	static	int sort_exampl_and_many(exType *arr, int size);
	static	int and_ex_set_reflective(exType *AND, exType *A, exType *B, int maxsize, int szA, int szB);
	static	int ex_and_op_many(wordType **wtarr, int  size);
	static	int comp_w_excnt(const void *A, const void *B);
	static	int sort_w_excnt(wordType **arr, int size);
	
	static	 int search_with_sentence_from_word_list(char *firstWord, wordType **wt, int wtSize, StrStream &stream, int searchMode=0);
	static	void	printExResult(exType *exList, int exNum, StrStream& stream);
	static	void	printResExResult(resExType *resExList, int exNum, StrStream& stream);
	static	void	debugPrintWordExList(exType *exList, int exNum, char *word);
	
	static	int placeExactFirst_ExList(char *firstWord, exType *exList, int exNum);
	static	int placeExactFirst_ResExList(char *firstWord, resExType *exList, int exNum);
	static	int ifSentenceHasThisWord(exType *ex, char *thisWord);
	static	void swapExType(exType *exA, exType *exB);
	static	void swapResExType(resExType *exA, resExType *exB);
	static	void makeLinkedExType(exType *exList, int exNum);
	
private:
	static  int search_simsentence_AND(char *firstWord, wordType** wt, int wtSize, StrStream& stream);
	static  int search_simsentence_counted_OR(char *firstWord, wordType** wt, int wtSize, StrStream& stream);
	static  int search_simsentence_counted_AND(char *firstWord, wordType** wt, int wtSize, StrStream& stream);
};

#endif
