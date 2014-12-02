#ifndef H_EXAM_H
#define H_EXAM_H
#include "HDict.h"
#include "simpleAlloc.h"
#include "StrStream.h"
#include "wordType.h"

class HExam
{
public:
	static	int	cmpEx(exType *exA, exType *exB);
	static	int	addEx(HwordType *word, exType *newex);
	static	void	printEx(exType *ex);
	static	void	printWordEx(HwordType *word);
	static	void	printExStc(exType *ex, StrStream &stream);
	static	void	printWordExStc(HwordType *word, StrStream &stream);
	static	void	printWordExStc_ExactFirst(char *queryWord, HwordType *word, StrStream &stream);
	static	int	loadWordEx_DB(simpleAlloc &mem);

	static	int comp_exampl_doc_id(const void *A, const void *B);
	static	int sort_exampl_doc_id(exType *arr, int size);
	static	int comp_exampl_and_many(const void *A, const void *B);
	static	int sort_exampl_and_many(exType *arr, int size);
	static	int and_ex_set_reflective(exType *AND, exType *A, exType *B, int maxsize, int szA, int szB);
	static	int ex_and_op_many(HwordType **wtarr, int  size);
	static	int comp_w_excnt(const void *A, const void *B);
	static	int sort_w_excnt(HwordType **arr, int size);
	
	static  int search_with_sentence_from_word_list(char *firstWord, HwordType **wt, int wtSize, StrStream &stream, int searchMode=0);


private:
	static  int search_simsentence_AND(char *firstWord, HwordType** wt, int wtSize, StrStream& stream);
	static  int search_simsentence_counted_OR(char *firstWord, HwordType** wt, int wtSize, StrStream& stream);
	static  int search_simsentence_counted_AND(char *firstWord, HwordType** wt, int wtSize, StrStream& stream);
};

#endif
