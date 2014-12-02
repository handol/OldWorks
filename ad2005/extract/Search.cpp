#include "ace/Log_Msg.h"

#include "Exam.h"
#include "Search.h"
#include "EDict.h"
#include "Docs.h"
#include "Exam.h"
#include "HExam.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "EDict.h"
#include "StrUtil.h"
#include "StrStream.h"
#include "dbStore.h"

#include "HStem.h"
#include "MemSplit.h"

simpleAlloc	Search::searchmem(1000);

/**
dbStore::prepare() �� �� �Լ� ȣ������ ���� �Ǿ�� �Ѵ�.
*/
int	Search::prepare(char *homepath)
{
		
	EDict::prepare(homepath);
	Exam::loadWordEx_DB(searchmem);
	Exam::loadIdiomEx_DB(searchmem);

	HDict::prepare(homepath);
	HExam::loadWordEx_DB(searchmem);
	
	Docs::load(homepath);
	return 0;
}

int	Search::finish()
{
	dbStore::finish();
	return 0;
}

int Search::dict(char *query, StrStream &stream)
{
	wordType *w = 0;
	byte	PS=0;
	
	w = EDict::search(query);
	if (w) {
		stream.add("WORD=%s\n\n", w->key);
		EDict::printWord(w, stream);
	}
	else {
		stream.add("Unknown word\n");
	}
	return 0;
}

/**
1. �־��� �˻��� (query) ���� �յ� ���� ����
2. ����/�ѱ� ����
3. �ܾ� ���� ����  (�ܾ�/����, �������� split)
4. stemming ���� 
*/
int Search::search(char *_query, StrStream &stream, int searchMode)
{
	char	query[256];
	wordType *w = 0;
	byte	PS=0;

	StrUtil::trim_left_right(query, sizeof(query)-1, _query);

	if ( (query[0] & 0x80) )
		searchHangul(query, stream);
	else
		searchEnglish(query, stream);

	return 0;
}

int Search::searchEnglish(char *query, StrStream &stream, int searchMode)
{
	wordType *w = 0;
	byte	PS=0;

	ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s mode=%d\n", query, searchMode));
	
	if (strchr(query, ' ')) {
	// ���� �ܾ�� �̷���� ���
		wordType	*words[W_IN_IDIOMS];
		int	cnt_words=0;
		idiomType *foundIdiom;
		char firstWord[64];
		
		// 2007.10.16 -- stemNwords() ���� -- ������ ���� �ܾ�� ����.
		cnt_words = EDict::stemNwords(words, W_IN_IDIOMS, query, firstWord);
		ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> %d English Words\n", query, cnt_words));

		if (cnt_words > 1) {		
			foundIdiom = EDict::searchIdiom(words, cnt_words);
			
			if (foundIdiom) {
				ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> Idiom found\n", query));
				Exam::printIdiomExStc(foundIdiom, stream);
			}
	 		else
			{
				ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> No Idiom ==> AND search\n", query));
				
				// Exam::search_with_sentence_from_word_list() ���� ���� ���� �Լ� �߿� �����Ѵ�.
				Exam::search_with_sentence_from_word_list(firstWord, words, cnt_words, stream, searchMode);
			}
		}
		else if (cnt_words==1)
		{
			ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> word found: %s ex=%d\n",
				query, words[0]->key, words[0]->numEx));
			searchEngOneWord(words[0],  stream, query);
		}
		else
		{
			ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> No word found\n", query));
			searchEngOneWord(NULL,  stream, query);
		}

		return 0;
		
	}
	else {
		// �� �ܾ�� �̷���� ���
		wordType *w = 0;
		byte	PS=0;
		w = EDict::stemWord(query, 0, &PS);
		if (w)
			ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> word found: %s ex=%d\n", query, w->key, w->numEx));
		else
			ACE_DEBUG((LM_INFO, "searchEnglish(): query=%s ==> No word found\n", query));
			
		searchEngOneWord(w,  stream);
	}

	

	return 0;
}

int Search::searchEngOneWord(wordType *w, StrStream &stream, char *firstWord)
{
	if (w) {
		stream.add("WORD=%s\n\n", w->key);
		EDict::printWord(w, stream);
		// ������ ���
		Exam::printWordExStc(w, stream);

	}
	else {
		stream.add("Unknown word\n");
	}
	return 0;
}

int Search::searchHangul(char *query, StrStream &stream, int searchMode)
{
	HwordType	*hwords[W_IN_IDIOMS];
	int	matchVals[W_IN_IDIOMS];
	int	cnt_words=0;

	char firstWord[64];
	
	cnt_words = Search::stemHangulWords(hwords, matchVals, W_IN_IDIOMS, query, firstWord);

	if (cnt_words==1)
	{
		// ������ ���
		if (matchVals[0] == OrgMatch)
		{
			HExam::printWordExStc(hwords[0], stream);	
		}
		else
		{
			HExam::printWordExStc_ExactFirst(firstWord, hwords[0], stream);
		}
	}
	else if (cnt_words  > 1)
	{
		ACE_DEBUG((LM_DEBUG, "searchHangul(): query=%s mode=%d\n",query, searchMode));
		HExam::search_with_sentence_from_word_list(firstWord, hwords, cnt_words, stream, searchMode);
	}
	else
	{
	}
	return 0;
}

/**
���� �ܾ�� ������ ��Ʈ���� �� �ܾ�� ������,  �� �ܾ��� stem �ܾ� ���� (WordType)
�� ���Ѵ�.

@param maxnum �ִ� �ܾ� ���� == W_IN_IDIOMS
*/
int	Search::stemHangulWords(HwordType	*hwordInfos[],  int matchVals[], int maxnum, char *buf, char *firstWord)
{	
	int	cnt_words=0;
	MemSplit	line(maxnum, 64);
	
	line.split(buf);	

	if (firstWord != NULL)
	{
		// ���� �ܾ� �� �� ù �ܾ �����ؼ� ������ ���´�. 2007.11.21
		strncpy(firstWord, line.str(0), 60);
	}
	
	for(int n=0; n<line.size(); n++) {
		HStem stemmer;
		stemmer.stem(line.str(n), line.len(n));
		stemResType *beststem = stemmer.getBestStem();
		if (beststem==NULL)
		{
			continue;
		}
		else
		{
			// 2007.10.22 - �ܾ 2 �̻��� ��   ������ ���� �ܾ�� ����. 
			if (cnt_words > 1 && beststem->stem->numEx==0)
				continue;
			hwordInfos[cnt_words] = beststem->stem;
			matchVals[cnt_words] = beststem->match;
			cnt_words++;
		}
	}
	return cnt_words;
}

