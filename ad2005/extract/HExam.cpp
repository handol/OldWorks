#include "ace/Log_Msg.h"

#include "HExam.h"
#include "HDict.h"
#include "Docs.h"
#include "StrUtil.h"
#include "DbQuery.h"
#include "dbStore.h"
#include "Exam.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

#define	PRN	printf
/**
���� ��� ������ ���� ��.
-- ���� ID, ���� ��ġ �������� ��.
*/
int	HExam::cmpEx(exType *exA, exType *exB)
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
int	HExam::addEx(HwordType *word, exType *ex, simpleAlloc &mem)
{
	exType *newex = 0;
	exType *prev=0, *iter = 0;
	int	cmp;
	newex = (exType *) mem.alloc(sizeof(exType));
	memcpy(newex, ex, sizeof(exType));

*/

/**
*/

int	HExam::addEx(HwordType *word, exType *newex)
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

void	HExam::printEx(exType *ex)
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

void	HExam::printExStc(exType *ex, StrStream &stream)
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

void	HExam::printWordEx(HwordType *word)
{
	exType *iter = 0;
	printf("%s : total %d Ex\n", word->key, word->numEx);
	for(iter = word->ex; iter; iter = iter->next) 
		printEx(iter);
}

void	HExam::printWordExStc(HwordType *word, StrStream &stream)
{
	exType *iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", word->key, word->numEx);
	for(iter = word->ex; iter; iter = iter->next) {
		printExStc(iter, stream);
	}
}

void	HExam::printWordExStc_ExactFirst(char *queryWord, HwordType *word, StrStream &stream)
{
	exType *iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n$QUERYWORD$ %s\n", word->key, word->numEx, queryWord);

	if (word->ex == NULL || word->numEx == 0)
		return;
		
	exType *sortedExList = NULL;
	sortedExList = (exType *) calloc(sizeof(exType), word->numEx);
	memcpy(sortedExList, word->ex, word->numEx * sizeof(exType));

	Exam::placeExactFirst_ExList(queryWord, sortedExList, word->numEx);
	
	for(iter = sortedExList; iter; iter = iter->next) {
		printExStc(iter, stream);
	}
}


int	HExam::loadWordEx_DB(simpleAlloc &mem)
{
	char	sqlbuf[128];
	exType ex;
	int	word_id;
	HwordType	*word=0;
	int	excnt=0, excnt_per_alpha;

	/*
	�� == 2
	�� == 3
	...
	�� == 20
	*/
	for(int ch=1; ch <= 20; ch++) {
		DbQuery	query;
		exType *newex=0;
		
		sprintf(sqlbuf, "select * from hExam_%d ", ch);
		if (query.exec(dbStore::dbConn, sqlbuf) < 0)
		{
			ACE_DEBUG((LM_INFO, "Cannot load table hExam_%d\n", ch));
			continue;
		}

		
		excnt_per_alpha = 0;
		while (query.fetch() ) {
			word_id = query.colIntVal(0);
			
			word = HDict::searchByID(word_id);
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

		ACE_DEBUG((LM_DEBUG, "[Hangul %d] Ex loaded: %d\n", ch, excnt_per_alpha));
	}
	ACE_DEBUG((LM_DEBUG, "Total Ex loaded: %d\n", excnt));
	return excnt;
}


/**
���� ����. ������ �ǹ̰� ���� ����.
���� �������� �ƴ��� ����.
*/
int HExam::comp_exampl_doc_id(const void *A, const void *B)
{
	int res=0;
	res = (uint4)(((exType*)A)->doc_id) - (uint4)(((exType*)B)->doc_id);
	if (res) return res;
	res = (uint4)(((exType*)A)->s_pos) - (uint4)(((exType*)B)->s_pos);
	return res;	
}


int HExam::sort_exampl_doc_id(exType *arr, int size)
{
	qsort(arr, size, sizeof(exType), comp_exampl_doc_id);
	return 0;
}

/**
3 �� �̻��� �ܾ ���� ���� ����.
�ܾ�鰣�� �Ÿ��� �������� �켱. (w_len)
������ ���̵��� �������� �켱. (level)
*/
int HExam::comp_exampl_and_many(const void *A, const void *B)
{
	int res=0;
	res = (uint4)(((exType*)A)->w_len) - (uint4)(((exType*)B)->w_len);
	if (res) return res;
	res = (uint4)(((exType*)A)->level) - (uint4)(((exType*)B)->level);
	return res;	
}


int HExam::sort_exampl_and_many(exType *arr, int size)
{
	qsort(arr, size, sizeof(exType), comp_exampl_and_many);
	return 0;
}


/*
2005.9.6
*/
// AND operation of sorted integer sets
// A, B must be sorted in ascending order [1, 2, ...,20]
int HExam::and_ex_set_reflective(exType *AND, exType *A, exType *B, int maxsize, int szA, int szB)
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
int HExam::ex_and_op_many(HwordType **wtarr, int  size)
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
int HExam::comp_w_excnt(const void *A, const void *B)
{
	int res;

	res = (uint4)( ((HwordType*)(*(int*)A))->numEx) - (uint4)( ((HwordType*)(*(int*)B))->numEx);
	return res;	
}


/**
���� ���� �� ���� ���� 
*/
int HExam::sort_w_excnt(HwordType **arr, int size)
{
	qsort(arr, size, sizeof(HwordType*), comp_w_excnt);
	return 0;
}

/**
�־��� HwordType ������� ���� ���� �˻��� �Ѵ�. 
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int HExam::search_with_sentence_from_word_list(char *firstWord, HwordType **wt, int wtSize, StrStream &stream, int searchMode)
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
�־��� HwordType ������� ���� ���� �˻��� �Ѵ�. 
����� AND operator�� �����Ѵ�.
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int HExam::search_simsentence_AND(char *firstWord, HwordType** wt, int wtSize, StrStream& stream)
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

	if (wtSize <= 0)
		return -1;

	// numEx�� �������� ������ �Ѵ�. ������ AND �˻� ����ȭ.
	sort_w_excnt(wt, wtSize);

	curEx = wt[idx]->ex;

	// ù word�� ������ ������ ���� ������ return
	if (wt[idx]->numEx <= 0)
		return 0;

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

	sort_exampl_doc_id(res, resSize);	// list merge�� ���� ����

	idx++;	// ���� word�� ����

	for (;idx < wtSize; ++idx) {
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
			int cmp = comp_exampl_doc_id((void *)&res[idxResOrg], (void *)&operand[idxOperand]);
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

	// ��ŷ
	sort_exampl_and_many(res, resSize);

	// ��� ���
	//iter = 0;
	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", wt[0]->key, resSize);	// �ϴ� Ű�� ù��° �ܾ��� ������ ����
	Exam::printExResult(res, resSize, stream);

ret:
	free(res);
	free(operand);
	return resSize;
}


int sort_exampl_doc_id_resExType_Hangul(resExType *arr, int size)
{
	qsort(arr, size, sizeof(resExType), HExam::comp_exampl_doc_id);
	return 0;
}
/**
2 �� �̻��� �ܾ ���� ���� ����.
�ܾ� ��ġ ���� ���� ���� �켱.
�ܾ�鰣�� �Ÿ��� �������� �켱. (w_len)
������ ���̵��� �������� �켱. (level)
*/
class CompExamplAndManyWithMtc_Hangul
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

int sort_exampl_and_many_with_mtc_Hangul(resExType *arr, int size)
{
	std::sort(arr, &arr[size], CompExamplAndManyWithMtc_Hangul());
	return 0;
}


/**
�־��� HwordType ������� ���� ���� �˻��� �Ѵ�. 
�� ������, OR operator�� ������� �� �����̴�. 
�켱 �� word�� ���� OR�� ������ ��, 
���� �� ������ �����Ѵ�. (��, ���� ��ũ�� AND ����� ���� �ȴ�.)
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int HExam::search_simsentence_counted_OR(char *firstWord, HwordType** wt, int wtSize, StrStream& stream)
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

	sort_exampl_doc_id_resExType_Hangul(res, resSize);	// list merge�� ���� ����
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
	sort_exampl_and_many_with_mtc_Hangul(res, resSize);
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
�־��� HwordType ������� ���� ���� �˻��� �Ѵ�. 
�� ���������� df�� 0�� �ƴ� ���� ���� word�� ��� �������� �Ͽ� 
���� �� ������ �����Ѵ�. (��, ���� ��ũ�� AND ����� ���� �ȴ�.)
��ȯ���� �����̸� ����, 
0 �̸� and ����� ����, 
����̸� and ��� ������ ũ���̴�.
*/
int HExam::search_simsentence_counted_AND(char *firstWord, HwordType** wt, int wtSize, StrStream& stream)
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

	if (wtSize <= 0)
		return -1;

	// numEx�� �������� ������ �Ѵ�. 
	sort_w_excnt(wt, wtSize);

	// ù word�� ������ ���� �ִ� word�� �̵�
	for (idx = 0; idx < wtSize && wt[idx]->numEx <= 0; ++idx);
	if (idx == wtSize)
		return 0;

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

	sort_exampl_doc_id_resExType_Hangul(res, resSize);	// list merge�� ���� ����
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;

	idx++;	// ���� word�� ����

	for (;idx < wtSize; ++idx) {
		curEx = wt[idx]->ex;
		curSize = wt[idx]->numEx;

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

	// ��ŷ
	sort_exampl_and_many_with_mtc_Hangul(res, resSize);
	for (i = 0; i < resSize - 1; ++i)
		res[i].next = &res[i+1];
	if (i > 0) res[resSize-1].next = NULL;

	// ��� ���

	stream.add("$WORD$ %s\n$NUMEX$ %d\n\n", wt[0]->key, resSize);	// �ϴ� Ű�� ù��° �ܾ��� ������ ����
	
	Exam::printResExResult(res, resSize, stream);

	if (res) free(res);
	if (operand) free(operand);
	return resSize;
}

#if 0
void	HExam::printExResult(exType *exList, int exNum, StrStream& stream)
{
	for (int i=0; i<exNum; i++)
	{
		printExStc(exList+i, stream);
	}
}

void	HExam::printResExResult(resExType *resExList, int exNum, StrStream& stream)
{
	for (int i=0; i<exNum; i++)
	{
		printExStc(resExList+i, stream);
	}
}

#endif

