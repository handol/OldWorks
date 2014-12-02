#include "ace/Log_Msg.h"

#include "Exam.h"
#include "EDict.h"
#include "Docs.h"
#include "StrUtil.h"
#include "DbQuery.h"
#include "dbStore.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#define	PRN	printf
/**
���� ��� ������ ���� ��.
-- ���� ID, ���� ��ġ �������� ��.
*/
int	Exam::cmpEx(exType *exA, exType *exB)
{
	int cmp=0;
	cmp = exA->level - exB->level;
	if (cmp != 0) return cmp;
	if (exA->doc_id < exB->doc_id) return -1;
	if (exA->doc_id > exB->doc_id) return 1;
	if (exA->s_pos < exB->s_pos) return -1;
	if (exA->s_pos > exB->s_pos) return 1;
	return 0;
}

/*
int	Exam::addEx(wordType *word, exType *ex, simpleAlloc &mem)
{
	exType *newex = 0;
	exType *prev=0, *iter = 0;
	int	cmp;
	newex = (exType *) mem.alloc(sizeof(exType));
	memcpy(newex, ex, sizeof(exType));

*/

/**
*/

int	Exam::addEx(wordType *word, exType *newex)
{
	exType *prev=0, *iter = 0;
	int	cmp;


	for(prev=0, iter = word->ex; iter != 0; prev= iter, iter = iter->next) {
		cmp = cmpEx(newex, iter);
		if (cmp < 0) break;
		if (cmp == 0) return 0;
	}

	if (prev){
		prev->next = newex;
		}
	else {
		word->ex = newex;
	}
	
	newex->next = iter;	

	word->numEx++;
	return 1;
}

int	Exam::addEx(idiomType *idiom, exType *newex)
{
	exType *prev=0, *iter = 0;
	int	cmp;


	for(prev=0, iter = idiom->ex; iter != 0; prev= iter, iter = iter->next) {
		cmp = cmpEx(newex, iter);
		if (cmp < 0) break;
		if (cmp == 0) return 0;
	}

	if (prev){
		prev->next = newex;
		}
	else {
		idiom->ex = newex;
	}
	
	newex->next = iter;	

	idiom->numEx++;
	return 1;
}

void	Exam::printEx(exType *ex)
{
	printf("%d %d %d %d %d %d %d\n",		
		ex->doc_id,
		ex->s_pos,
		ex->s_len,
		ex->w_pos,
		ex->w_len,
		ex->level
		);
}

void	Exam::printExStc(exType *ex, StrStream &stream)
{
	docType *doc;
	char	sentence[1024];
	int	stc_len;
	doc = Docs::searchDoc(ex->doc_id);
	
	if (doc && doc->size > 0 && doc->text) {
		stc_len = MIN(sizeof(sentence)-1, ex->s_len) ;
		strncpy(sentence, 
				doc->text + ex->s_pos,
				stc_len);
		sentence[stc_len] = 0;
		stream.add("$DOCID$ %d\n", doc->doc_id);
		stream.add("$LVL$ %d\n", ex->level);
		stream.add("$URL$ %s\n", doc->url);
		stream.add("$STC$ %s\n", sentence);	
	}
	else {
		stream.add("NO doc\n");	
	}
	stream.add("\n");
}

void	Exam::printWordEx(wordType *word)
{
	exType *iter = 0;
	printf("%s : total %d Ex\n", word->key, word->numEx);
	for(iter = word->ex; iter; iter = iter->next) 
		printEx(iter);
}

void	Exam::printWordExStc(wordType *word, StrStream &stream)
{
exType *iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", word->key, word->numEx);
	for(iter = word->ex; iter; iter = iter->next) {
		printExStc(iter, stream);
	}
}

void	Exam::printIdiomExStc(idiomType *idiom, StrStream &stream)
{
exType *iter = 0;
	stream.add("$IDIOM$ %s\n$NUMEX$ %d\n\n", idiom->key, idiom->numEx);
	for(iter = idiom->ex; iter; iter = iter->next) {
		printExStc(iter, stream);
	}
}

int	Exam::loadWordEx(char *homepath, simpleAlloc &mem)
{
	FILE	*fp;
	char	buf[1024];
	char	fpath[256];
	exType *ex;
	int	word_id;
	wordType	*word;
	int	excnt=0;

	StrUtil::path_merge(fpath, homepath, "word.ex");
	if ( (fp = fopen(fpath, "r"))== NULL) return -1;
	
	while (fgets(buf, 256, fp) != NULL) {
		ex = (exType *) mem.alloc(sizeof(exType));
		sscanf(buf, "%d %d %d %d %d %d %d", 
			&word_id,
			&ex->doc_id,
			&ex->s_pos,
			&ex->s_len,
			&ex->w_pos,
			&ex->w_len,
			&ex->level
		);
		word = EDict::searchWord(word_id);
		if (word) {
			addEx(word, ex);
			excnt++;
		}
	}

	printf("Ex loaded: %d\n", excnt);
	ACE_DEBUG((LM_INFO, "Ex loaded: %d\n", excnt));
	fclose(fp);
	return excnt;
}


int	Exam::loadWordEx_DB(simpleAlloc &mem)
{
	char	sqlbuf[128];
	exType ex;
	int	word_id;
	wordType	*word;
	int	excnt=0, excnt_per_alpha;

	for(int ch='A'; ch <= 'Z'; ch++) {
		DbQuery	query;
		exType *newex=0;
		
		sprintf(sqlbuf, "select * from wExam_%c ", ch);
		if (query.exec(dbStore::dbConn, sqlbuf) < 0)
		{
			ACE_DEBUG((LM_INFO, "Cannot load table wExam_%c\n", ch));
			continue;
		}
		
		excnt_per_alpha = 0;
		while (query.fetch() ) {
			word_id = query.colIntVal(0);
			
			word = EDict::searchWord(word_id);
			if (word==NULL) continue;

			memset(&ex, 0, sizeof(ex));
			ex.doc_id = query.colIntVal(1);
			if (ex.doc_id==0) continue;
			
			ex.s_pos = query.colIntVal(2);
			ex.s_len = query.colIntVal(3);
			ex.w_pos = query.colIntVal(4);
			ex.w_len = query.colIntVal(5);
			ex.level = query.colIntVal(6);			
			
			if (word) {
				newex = (exType *) mem.copyAlloc( (void*)&ex, sizeof(exType));
				addEx(word, newex);
				excnt_per_alpha++;
				excnt++;
			}
		}

		ACE_DEBUG((LM_DEBUG, "[%c] Ex loaded: %d\n", ch, excnt_per_alpha));
	}
	ACE_DEBUG((LM_DEBUG, "Total Ex loaded: %d\n", excnt));
	return excnt;
}



int	Exam::loadIdiomEx_DB(simpleAlloc &mem)
{
	char	sqlbuf[128];
	exType ex;
	int	idiom_id;
	idiomType	*idiom;
	int	excnt=0, excnt_per_alpha;

	for(int ch='A'; ch <= 'Z'; ch++) {
		DbQuery	query;
		exType *newex=0;
		
		sprintf(sqlbuf, "select * from iExam_%c ", ch);
		if (query.exec(dbStore::dbConn, sqlbuf) < 0)
		{
			ACE_DEBUG((LM_INFO, "Cannot load table iExam_%c\n", ch));
			continue;
		}
		
		excnt_per_alpha = 0;
		while (query.fetch() ) {
			idiom_id = query.colIntVal(0);
			
			idiom = EDict::searchIdiom(idiom_id);
			if (idiom==NULL) continue;

			memset(&ex, 0, sizeof(ex));
			ex.doc_id = query.colIntVal(1);
			if (ex.doc_id==0) continue;
			
			ex.s_pos = query.colIntVal(2);
			ex.s_len = query.colIntVal(3);
			ex.w_pos = query.colIntVal(4);
			ex.w_len = query.colIntVal(5);
			ex.level = query.colIntVal(6);			
			
			if (idiom) {
				newex = (exType *) mem.copyAlloc( (void*)&ex, sizeof(exType));
				addEx(idiom, newex);
				excnt_per_alpha++;
				excnt++;
			}
		}

		ACE_DEBUG((LM_DEBUG, "[%c] Ex loaded: %d\n", ch, excnt_per_alpha));
	}
	ACE_DEBUG((LM_DEBUG, "Total Ex loaded: %d\n", excnt));
	return excnt;
}


/**
���� ����. ������ �ǹ̰� ���� ����.
���� �������� �ƴ��� ����.
*/
int Exam::comp_exampl_doc_id(const void *A, const void *B)
{
	int res=0;
	res = (uint4)(((exType*)A)->doc_id) - (uint4)(((exType*)B)->doc_id);
	if (res) return res;
	res = (uint4)(((exType*)A)->s_pos) - (uint4)(((exType*)B)->s_pos);
	return res;	
}


int Exam::sort_exampl_doc_id(exType *arr, int size)
{
	qsort(arr, size, sizeof(exType), comp_exampl_doc_id);
	return 0;
}

/**
3 �� �̻��� �ܾ ���� ���� ����.
�ܾ�鰣�� �Ÿ��� �������� �켱. (w_len)
������ ���̵��� �������� �켱. (level)
*/
int Exam::comp_exampl_and_many(const void *A, const void *B)
{
	int res=0;
	res = (uint4)(((exType*)A)->w_len) - (uint4)(((exType*)B)->w_len);
	if (res) return res;
	res = (uint4)(((exType*)A)->level) - (uint4)(((exType*)B)->level);
	return res;	
}


int Exam::sort_exampl_and_many(exType *arr, int size)
{
	qsort(arr, size, sizeof(exType), comp_exampl_and_many);
	return 0;
}


/*
2005.9.6
*/
// AND operation of sorted integer sets
// A, B must be sorted in ascending order [1, 2, ...,20]
int Exam::and_ex_set_reflective(exType *AND, exType *A, exType *B, int maxsize, int szA, int szB)
{
	int idxA=0, idxB=0, n=0,sum;
	int cmp=0;
	int	diff=0;
	sum = szA + szB;
	for(n=0; idxA < szA && idxB < szB && n<maxsize; ) {
		cmp = comp_exampl_doc_id((void *)A, (void *)B);
		if (cmp==0) {
			memcpy(AND, A, sizeof(exType));
			if (A->w_pos  < B->w_pos) {
				diff = B->w_pos - A->w_pos;
				AND->w_pos = A->w_pos;
				AND->w_len = diff + B->w_len;
			}
			else if (A->w_pos  > B->w_pos) {
				diff = A->w_pos - B->w_pos;
				AND->w_pos = B->w_pos;
				AND->w_len = diff + A->w_len;
			}
			
			A++;
			B++;
			idxA++;			
			idxB++;
			if (A->s_len < 200) {
				AND++;
				n++;
			}
		}
		else if (cmp<0) {
			A++; idxA++; 
		} 
		else { 
			B++; idxB++; 
		}
	}
	return n; // size of result of AND

}


/**
3 �ܾ� �̻� ���� and ���� 

@todo
 ex �� ������ �� filtering(category ����) �Ǵ� memcpy()  �� �����ϵ��� �Լ� �ۼ� �ʿ�.
*/
int Exam::ex_and_op_many(wordType **wtarr, int  size)
{
	int	i;
	int	index;
	int	and_size;
	exType  *AND;

	/*  ������ ���� �ܾ�� AND ���� �����Ѵ�.
	   1. ���� ���� �� ���� �ܾ ���� 
	   2. ������ ���� �ܾ�� skip. 
	   3. and_size == �ּ� ���� ����
	 */
	sort_w_excnt(wtarr, size);
	
	for(i=0; i<size; i++) {
		if (wtarr[i]->numEx > 0) {
			index = i;
			and_size = wtarr[i]->numEx;
			break;
		}
	}

	if (and_size==0 || size - index < 2) return 0;

	
	//AND = (exType *)calloc(sizeof(exType), and_size);

	exType *ex1=0, *ex2=0;
	int	sz1=0, sz2=0;

	AND = wtarr[index]->ex;
	and_size = wtarr[index]->numEx;
		
	for(i=index+1; i<size; i++) {
		sz1 = and_size;
		sz2 = wtarr[i]->numEx;	
		if (sz1==0 || sz2==0 || wtarr[i]->ex==0 ) return 0;
		ex1 = (exType *)calloc(sizeof(exType), sz1);
		ex2 = (exType *)calloc(sizeof(exType), sz2);
		
		
		memcpy(ex1, AND, sizeof(exType)*sz1);
		memcpy(ex2, wtarr[i]->ex, sizeof(exType)*sz2);

		if (i==index+1) 
			sort_exampl_doc_id(ex1, sz1);
		sort_exampl_doc_id(ex2, sz2);

		if (i > index+1) {
			free(AND);
			AND = 0;
		}
		
		and_size = MIN(sz1, sz2);
		AND = (exType *)calloc(sizeof(exType), and_size);
		
		and_size = and_ex_set_reflective(AND, ex1, ex2, and_size, sz1, sz2);
		
		if (ex1) free(ex1);
		if (ex2) free(ex2);
	
	}

	sort_exampl_and_many(AND, and_size);
	//prn_ex_list_part(AND, size);
	if (AND) free(AND);
	return and_size;
}


/**
���� ���� �� ���� ���� 
*/
int Exam::comp_w_excnt(const void *A, const void *B)
{
	int res;

	res = (uint4)( ((wordType*)(*(int*)A))->numEx) - (uint4)( ((wordType*)(*(int*)B))->numEx);
	return res;	
}


/**
���� ���� �� ���� ���� 
*/
int Exam::sort_w_excnt(wordType **arr, int size)
{
	qsort(arr, size, sizeof(wordType*), comp_w_excnt);
	return 0;
}

/**
�־��� wordType ������� ���� ���� �˻��� �Ѵ�. 
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int Exam::search_with_sentence_from_word_list(char *firstWord, wordType **wt, int wtSize, StrStream &stream, int searchMode)
{
	if (searchMode > 0)
	{
		/*search_simsentence_counted_AND -  ���� ���絵 �˻� ��Ÿ�� ����. �˰��� ���� 
		   AND �� ����� ���� ���, ������ ��� �����.
		*/
		return search_simsentence_counted_AND(firstWord, wt, wtSize, stream);
	}
	
	else
	{
		/*search_simsentence_AND -  �ܾ ������ �϶� AND ������ �˻�. �˰��� �ܼ�.
		*/
		return search_simsentence_AND(firstWord, wt, wtSize, stream);
	}
}

/**
�־��� wordType ������� ���� ���� �˻��� �Ѵ�. 
����� AND operator�� �����Ѵ�.
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int Exam::search_simsentence_AND(char *firstWord, wordType** wt, int wtSize, StrStream& stream)
{
	int i;
	int idx = 0;
	int curSize = 0;
	exType *curEx = NULL;
	int resSize = 0;
	exType *res = NULL;
	exType *operand = NULL;
	exType *exIter = NULL;

	// ��� ���
	exType *iter = 0;
	int	cnt = 0;

	ACE_DEBUG((LM_INFO, "search_simsentence_AND(): #word = %d\n", wtSize));
	for (i=0; i<wtSize; i++)
	{
		ACE_DEBUG((LM_INFO, "search_simsentence_AND(): word[%d] = %s, numEx=%d\n", 
			i+1, wt[i]->key, wt[i]->numEx));
	}
	
	if (wtSize <= 0)
		return -1;

	// numEx�� �������� ������ �Ѵ�. ������ AND �˻� ����ȭ.
	sort_w_excnt(wt, wtSize);

	for (i=0; i<wtSize; i++)
	{
		ACE_DEBUG((LM_INFO, "search_simsentence_AND(): sort: word[%d] = %s, numEx=%d\n", 
			i+1, wt[i]->key, wt[i]->numEx));
	}
	

	// ù word�� ������ ������ ���� ������ return
	if (wt[idx]->numEx <= 0)
	{
		ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): numEx==0 after sorting\n"));
		return 0;
	}

	curEx = wt[idx]->ex;
	resSize = wt[idx]->numEx;
	res = (exType*) malloc(sizeof(exType)*resSize);	// ��� �迭�� capacity�� ù word�� ���� ������ �Ѵ�. AND �˻��� �̰� �ִ�ġ�̱� ����
	if (res == NULL)
		return -1;
	operand = (exType*) malloc(sizeof(exType)*wt[wtSize-1]->numEx); // operand �迭�� capacity�� ������ word�� ���� ������ �Ѵ�. �̰� ���� ū �迭�̴�.
	if (operand == NULL) {
		free(res);
		return -1;
	}

	for (i = 0, exIter = curEx; i < resSize && exIter != NULL; ++i, exIter = exIter->next) {
		memcpy(&(res[i]), exIter, sizeof(exType));
		res[i].next = &res[i+1];
	}
	if (i > 0) res[i-1].next = NULL;

	ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): resSize=%d\n",  resSize));
	
	sort_exampl_doc_id(res, resSize);	// list merge�� ���� ����
	debugPrintWordExList(res, resSize, wt[idx]->key);

	idx++;	// ���� word�� ����

	for (;idx < wtSize; ++idx) {
		curEx = wt[idx]->ex;
		curSize = wt[idx]->numEx;

		ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): merge: idx=%d word=%s numEx=curSize=%d\n",
			idx, wt[idx]->key, wt[idx]->numEx));

		debugPrintWordExList(curEx, curSize, wt[idx]->key);
		
		for (i = 0; i < wt[idx]->numEx; i++) {	// operand�� ����. �̰� ���� �� ���� ��
			memcpy( &operand[i], curEx+i, sizeof(exType) );
		}
		

		ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): resSize=%d\n", resSize));
		sort_exampl_doc_id(operand, curSize);

		debugPrintWordExList(operand, wt[idx]->numEx, wt[idx]->key);

		// list merge
		int idxResOrg = 0, idxOperand = 0;
		int idxRes = 0;

		ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): while loop\n"));
		int	loopcnt = 0;
		while(idxResOrg < resSize && idxOperand < curSize) {
			int cmp = comp_exampl_doc_id((void *)&res[idxResOrg], (void *)&operand[idxOperand]);
			if (loopcnt++ < 10)
			ACE_DEBUG((LM_DEBUG, "\tmerge loop[%d]: idxResOrg=%d idxOperand=%d cmp=%d\n",
				loopcnt, idxResOrg, idxOperand, cmp));	
			if (cmp == 0) {
				if (idxRes <= idxResOrg) {
					if (idxRes != idxResOrg)
						memcpy( &res[idxRes], &res[idxResOrg], sizeof(exType) );
					idxRes++;
					idxResOrg++;
					idxOperand++;
				}
			}
			else if (cmp < 0) {
				idxResOrg++;
			}
			else {
				idxOperand++;
			}
		}

		resSize = idxRes;
		if (idxRes > 0) {
			res[idxRes-1].next = NULL;
		}
		else {
			goto ret;
		}
	}

	ACE_DEBUG((LM_DEBUG, "search_simsentence_AND(): calling sort_exampl_and_many(): resSize=%d\n", resSize));
	// ��ŷ
	sort_exampl_and_many(res, resSize);

	// ��� ���
	//iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", wt[0]->key, resSize);	// �ϴ� Ű�� ù��° �ܾ��� ������ ����
	printExResult(res, resSize, stream);

ret:
	free(res);
	free(operand);
	return resSize;
}


int sort_exampl_doc_id_resExType(resExType *arr, int size)
{
	qsort(arr, size, sizeof(resExType), Exam::comp_exampl_doc_id);
	return 0;
}
/**
2 �� �̻��� �ܾ ���� ���� ����.
�ܾ� ��ġ ���� ���� ���� �켱.
�ܾ�鰣�� �Ÿ��� �������� �켱. (w_len)
������ ���̵��� �������� �켱. (level)
*/
class CompExamplAndManyWithMtc
{
public:
	bool operator()(const resExType& a, const resExType& b) const {
		int cmp = 0;
		cmp = a.matchedTermCnt - b.matchedTermCnt;
		if (cmp != 0)
			return cmp > 0;
		cmp = (uint4)(a.w_len) - (uint4)(b.w_len);
		if (cmp != 0)
			return cmp < 0;
		return a.level <= b.level;
	}
};

int sort_exampl_and_many_with_mtc(resExType *arr, int size)
{
	std::sort(arr, &arr[size], CompExamplAndManyWithMtc());
	return 0;
}


/**
�־��� wordType ������� ���� ���� �˻��� �Ѵ�. 
�� ������, OR operator�� ������� �� �����̴�. 
�켱 �� word�� ���� OR�� ������ ��, 
���� �� ������ �����Ѵ�. (��, ���� ��ũ�� AND ����� ���� �ȴ�.)
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int Exam::search_simsentence_counted_OR(char *firstWord, wordType** wt, int wtSize, StrStream& stream)
{
	int i;
	int idx = 0;
	int curSize = 0;
	exType *curEx = NULL;
	int resSize = 0;
	resExType *res = NULL;			// ��� ���� �迭
	resExType *resOperand = NULL;	// ��� ���� �迭. res�� resOperand�� ���� ������ ������ ����, ù��° operand�� ������� �����ϴ� ������ �Ѵ�.
	exType *operand = NULL;
	exType *exIter = NULL;
	int maxResSize = 0;
	// ��� ���
	exType *iter = 0;
	int maxMTC = 1;

	if (wtSize <= 0)
		return -1;

	// ������� �ִ� capacity�� ���Ѵ�.
	for (i = 0; i < wtSize; ++i) {
		maxResSize += wt[i]->numEx;
	}

	// numEx�� �������� ������ �Ѵ�. 
	sort_w_excnt(wt, wtSize);

	// ù word�� ������ ���� �ִ� word�� �̵�
	for (idx = 0; idx < wtSize && wt[idx]->numEx <= 0; ++idx);
	if (idx == wtSize)
		return 0;

	curEx = wt[idx]->ex;

	resSize = wt[idx]->numEx;
	res = (resExType*) malloc(sizeof(resExType)*maxResSize);	
	if (res == NULL)
		return -1;
	resOperand = (resExType*) malloc(sizeof(resExType)*maxResSize);
	if (resOperand == NULL) {
		free(res);
		return -1;
	}
	operand = (exType*) malloc(sizeof(exType)*wt[wtSize-1]->numEx); // operand �迭�� capacity�� ������ word�� ���� ������ �Ѵ�. �̰� ���� ū �迭�̴�.
	if (operand == NULL) {
		free(res);
		free(resOperand);
		return -1;
	}

	// ù word�� doc list�� ��� list�� ����
	for (i = 0, exIter = curEx; i < resSize && exIter != NULL; ++i, exIter = exIter->next) {
		memcpy(&(res[i]), exIter, sizeof(exType));
		res[i].next = &res[i+1];
		res[i].matchedTermCnt = 1;
	}
	if (i > 0) res[i-1].next = NULL;

	sort_exampl_doc_id_resExType(res, resSize);	// list merge�� ���� ����
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;

	idx++;	// ���� word�� ����

	for (;idx < wtSize; ++idx) {
		// res�� resOperand�� swap
		resExType* tmp = resOperand;
		resOperand = res;
		res = tmp;

		curEx = wt[idx]->ex;
		curSize = wt[idx]->numEx;

		for (i = 0; i < wt[idx]->numEx; i++) {	// operand�� ����. �̰� ���� �� ���� ��
			memcpy( &operand[i], curEx+i, sizeof(exType) );
		}

		sort_exampl_doc_id(operand, curSize);

		// list merge
		int idxResOrg = 0, idxOperand = 0;
		int idxRes = 0;

		while(idxResOrg < resSize && idxOperand < curSize) {
			int cmp = comp_exampl_doc_id((void *)&resOperand[idxResOrg], (void *)&operand[idxOperand]);
			if (cmp == 0) {
				memcpy( &res[idxRes], &resOperand[idxResOrg], sizeof(exType) );
				res[idxRes].matchedTermCnt = resOperand[idxResOrg].matchedTermCnt + 1;
				if (res[idxRes].matchedTermCnt > maxMTC)
					maxMTC = res[idxRes].matchedTermCnt;
				res[idxRes].next = &res[idxRes+1];
				idxRes++;
				idxResOrg++;
				idxOperand++;
			}
			else if (cmp < 0) {
				memcpy( &res[idxRes], &resOperand[idxResOrg], sizeof(exType) );
				res[idxRes].matchedTermCnt = 1;
				res[idxRes].next = &res[idxRes+1];
				idxRes++;
				idxResOrg++;
			}
			else {
				memcpy( &res[idxRes], &operand[idxOperand], sizeof(exType) );
				res[idxRes].matchedTermCnt = 1;
				res[idxRes].next = &res[idxRes+1];
				idxRes++;
				idxOperand++;
			}
		}
		while (idxResOrg < resSize) {
			memcpy( &res[idxRes], &resOperand[idxResOrg], sizeof(exType) );
			res[idxRes].matchedTermCnt = 1;
			res[idxRes].next = &res[idxRes+1];
			idxRes++;
			idxResOrg++;
		}
		while (idxOperand < curSize) {
			memcpy( &res[idxRes], &operand[idxOperand], sizeof(exType) );
			res[idxRes].matchedTermCnt = 1;
			res[idxRes].next = &res[idxRes+1];
			idxRes++;
			idxOperand++;
		}

		resSize = idxRes;
		if (idxRes > 0) {
			res[idxRes-1].next = NULL;
		}
		else {
			goto ret;
		}
	}

	// ��ŷ
	sort_exampl_and_many_with_mtc(res, resSize);
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;

	// ��� ���
	iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", wt[0]->key, resSize);	// �ϴ� Ű�� ù��° �ܾ��� ������ ����
	for(iter = res; iter; iter = iter->next) {
		if (((resExType*)iter)->matchedTermCnt <= maxMTC/2)
			break;
		printExStc(iter, stream);
		//printf("%d %d\n", iter->doc_id, ((resExType*)iter)->matchedTermCnt);
	}

ret:
	if (res) free(res);
	if (resOperand) free(resOperand);
	if (operand) free(operand);
	return resSize;
}

/**
�־��� wordType ������� ���� ���� �˻��� �Ѵ�. 
�� ���������� df�� 0�� �ƴ� ���� ���� word�� ��� �������� �Ͽ� 
���� �� ������ �����Ѵ�. (��, ���� ��ũ�� AND ����� ���� �ȴ�.)
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int Exam::search_simsentence_counted_AND(char *firstWord, wordType** wt, int wtSize, StrStream& stream)
{
	int i;
	int idx = 0;
	int curSize = 0;
	exType *curEx = NULL;
	int resSize = 0;
	resExType *res = NULL;			// ��� ���� �迭
	exType *operand = NULL;
	exType *exIter = NULL;
	// ��� ���
	exType *iter = 0;

	ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): #word = %d\n", wtSize));
	for (i=0; i<wtSize; i++)
	{
		ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): word[%d] = %s, numEx=%d\n", 
			i+1, wt[i]->key, wt[i]->numEx));
	}
	if (wtSize <= 0)
		return -1;

	// numEx�� �������� ������ �Ѵ�. 
	sort_w_excnt(wt, wtSize);

	for (i=0; i<wtSize; i++)
	{
		ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): sort: word[%d] = %s, numEx=%d\n", 
			i+1, wt[i]->key, wt[i]->numEx));
	}

	// ù word�� ������ ���� �ִ� word�� �̵�
	for (idx = 0; idx < wtSize && wt[idx]->numEx <= 0; ++idx);
	if (idx == wtSize)
	{
		ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): numEx==0 after sorting\n"));
		return 0;
	}

	curEx = wt[idx]->ex;

	resSize = wt[idx]->numEx;
	res = (resExType*) malloc(sizeof(resExType)*resSize);	
	if (res == NULL)
		return -1;
	operand = (exType*) malloc(sizeof(exType)*wt[wtSize-1]->numEx); // operand �迭�� capacity�� ������ word�� ���� ������ �Ѵ�. �̰� ���� ū �迭�̴�.
	if (operand == NULL) {
		free(res);
		return -1;
	}

	// ù word�� doc list�� ��� list�� ����
	for (i = 0, exIter = curEx; i < resSize && exIter != NULL; ++i, exIter = exIter->next) {
		memcpy(&(res[i]), exIter, sizeof(exType));
		res[i].next = &res[i+1];
		res[i].matchedTermCnt = 1;
	}
	if (i > 0) res[i-1].next = NULL;

	sort_exampl_doc_id_resExType(res, resSize);	// list merge�� ���� ����

	/*
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;
	*/
	makeLinkedExType(res, resSize);

	idx++;	// ���� word�� ����

	for (;idx < wtSize; ++idx) {
		
		curEx = wt[idx]->ex;
		curSize = wt[idx]->numEx;

		ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): merge: idx=%d word=%s numEx=%d\n",
			idx, wt[idx]->key, wt[idx]->numEx));
			

		for (i = 0; i < wt[idx]->numEx; i++) {	// operand�� ����. �̰� ���� �� ���� ��
			memcpy( &operand[i], curEx+i, sizeof(exType) );
		}

		sort_exampl_doc_id(operand, curSize);

		// list merge
		int idxResOrg = 0, idxOperand = 0;

		while(idxResOrg < resSize && idxOperand < curSize) {
			int cmp = comp_exampl_doc_id((void *)&res[idxResOrg], (void *)&operand[idxOperand]);
			if (cmp == 0) {
				res[idxResOrg].matchedTermCnt +=1;;
				idxResOrg++;
				idxOperand++;
			}
			else if (cmp < 0) {
				idxResOrg++;
			}
			else {
				idxOperand++;
			}
		}
	}

	ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): resSize=%d\n", resSize));
	for (i=0; i<resSize; i++)
	{
		ACE_DEBUG((LM_DEBUG, "search_simsentence_counted_AND(): resExType:  doc=%d, s_pos=%d match=%d\n", 
			res[i].doc_id, res[i].s_pos, res[i].matchedTermCnt ));
	}
	
	// ��ŷ
	// 2007.10.16  ���� ������ �ǽ�.  "to the" �˻���.
	sort_exampl_and_many_with_mtc(res, resSize);
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;

	// ��� ���
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", wt[0]->key, resSize);	// �ϴ� Ű�� ù��° �ܾ��� ������ ����

	printResExResult(res, resSize, stream);

	if (res) free(res);
	if (operand) free(operand);
	return resSize;
}

void	Exam::printExResult(exType *exList, int exNum, StrStream& stream)
{
	for (int i=0; i<exNum; i++)
	{
		printExStc(exList+i, stream);
	}
}

void	Exam::printResExResult(resExType *resExList, int exNum, StrStream& stream)
{
	for (int i=0; i<exNum; i++)
	{
		printExStc(resExList+i, stream);
		//printf("%d %d\n", iter->doc_id, ((resExType*)iter)->matchedTermCnt);
	}
}

void	Exam::debugPrintWordExList(exType *exList, int exNum, char *word)
{
	ACE_DEBUG((LM_DEBUG, "== Word: %s  Ex: %d\n", word,  exNum));
		
	for (int i=0; i<exNum; i++)
		ACE_DEBUG((LM_DEBUG, "%d-%d\n", exList[i].doc_id, exList[i].s_pos));

	/*
	exType *iter = 0;
	for(iter = word->ex; iter; iter = iter->next) 
		ACE_DEBUG((LM_DEBUG, "%d-%d\n", iter->doc_id, iter->s_pos));
	*/
	ACE_DEBUG((LM_DEBUG, "\n\n"));	
}

/**
�־���, �ֽ��ϴ�, �־��µ� ������ �˻��� ��쿡 ��� ���� ������ ���� �Լ�.
"�־���" �� �˻��ϸ� �ٸ� ������麸�� "�־���"�� ���Ե� ������ ���� �տ� �����ش�.

���� ���:

input ���� )
���� ���:   B, A, B, B, C, D, A, B
�˻��� : B

�����)
B B B B A C D A

*/
int Exam::placeExactFirst_ExList(char *firstWord, exType *exList, int exNum)
{
	int wordMatchCount = 0;
	for (int i=0; i<exNum; i++)
	{
		if (ifSentenceHasThisWord(&(exList[i]), firstWord))
		{
			if (wordMatchCount < i)
			{
				// do swap
				swapExType(&(exList[wordMatchCount]), &(exList[i]));	
			}
			wordMatchCount++;
		}
	}

	makeLinkedExType(exList, exNum);
	return 0;
}

int Exam::placeExactFirst_ResExList(char *firstWord, resExType *exList, int exNum)
{
	int wordMatchCount = 0;
	for (int i=0; i<exNum; i++)
	{
		if (ifSentenceHasThisWord(&(exList[i]), firstWord))
		{
			if (wordMatchCount < i)
			{
				// do swap
				swapResExType(&(exList[wordMatchCount]), &(exList[i]));	
			}
			wordMatchCount++;
		}
	}

	makeLinkedExType(exList, exNum);
	return 0;
}

/**
�־��� ������ �־��� �ܾ �����ϴ��� �˻�.  �ִ�/�ֽ��ϴ� ������ �����ϱ� ���� ��.
return 1: if a given ex's sentence includes a given word 'thisWord'
return 0: else
2007.11.22
*/
int Exam::ifSentenceHasThisWord(exType *ex, char *thisWord)
{
	docType *doc;
	char	sentence[1024];
	int	stc_len;
	doc = Docs::searchDoc(ex->doc_id);
	
	if (doc && doc->size > 0 && doc->text) {
		stc_len = MIN(sizeof(sentence)-1, ex->s_len) ;
		strncpy(sentence, 
				doc->text + ex->s_pos,
				stc_len);
		sentence[stc_len] = 0;
		if (strstr(sentence, thisWord) != NULL)
			return 1;
		else
			return 0;
	}
	else {
		return 0;
	}
}

void Exam::swapExType(exType *exA, exType *exB)
{
	exType tmp;
	memcpy(&tmp, exA, sizeof(exType));
	memcpy(exA, exB, sizeof(exType));
	memcpy(exB, &tmp, sizeof(exType));
}

void Exam::swapResExType(resExType *exA, resExType *exB)
{
	resExType tmp;
	memcpy(&tmp, exA, sizeof(resExType));
	memcpy(exA, exB, sizeof(resExType));
	memcpy(exB, &tmp, sizeof(resExType));
}

void Exam::makeLinkedExType(exType *exList, int exNum)
{
	for (int i = 0; i < exNum - 1; ++i)
		exList[i].next = &exList[i+1];
	if (exNum > 0) 
		exList[exNum-1].next = NULL;
}

