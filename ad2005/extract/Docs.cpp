#include "ace/Log_Msg.h"

#include "Exam.h"
#include "Docs.h"
#include "EDict.h"
#include "FileUtil.h"
#include "DbQuery.h"
#include "dbStore.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "StrUtil.h"

#define	PRN	printf
simpleAlloc	Docs::docmem(1000);

docType	*Docs::docArray = 0;

int	Docs::docArraySize = 0;
int	Docs::docNums = 0;

int	Docs::load(char *homepath)
{
	char fpath[256];

	// 중복 로드 방지.
	if (docArray) return 0;
	
	StrUtil::path_merge(fpath, homepath, "docinfo.dat");
	
	load_docinfo();
	load_doctext();
	return 0;
}


int	Docs::load_docinfo()
{
	int	max_doc_id = 10;
	int	n;
	DbQuery	query;
	docType *doc;
	
	max_doc_id = 	dbStore::getMaxValue("docs", "id");
	ACE_DEBUG((LM_INFO, "MAX doc_id = %d\n", max_doc_id));
	
	if (max_doc_id == 0) {		
		return 0;
	}

	docArraySize = max_doc_id + 1;
	//docArray = (docType *) docmem.alloc( (max_doc_id+1) * sizeof(docType) );
	docArray = new docType [ docArraySize ];

	memset(docArray, 0, sizeof(docType) * docArraySize);
	
	query.exec(dbStore::dbConn, "select * from docs order by id");
	n=0;
	while (query.fetch()) {
		int	doc_id=0;
		
		doc_id = query.colIntVal(0);
		if (doc_id <= 0 || doc_id > max_doc_id) {
			PRN("Wrong doc_id (%d)\n", doc_id);
			continue;
		}
		doc = docArray + doc_id;
		doc->doc_id = doc_id;
		doc->url = docmem.strAlloc(query.colStrVal_trimmed(1) );
		doc->title = docmem.strAlloc(query.colStrVal_trimmed(2) );
		doc->fpath = docmem.strAlloc(query.colStrVal_trimmed(3) );
		doc->size = query.colIntVal(5);
		doc->processed = query.colIntVal(6);
		doc->level = query.colIntVal(7);
		print(doc);
		docNums++;
		
	}

	//query.info();
	
	ACE_DEBUG((LM_INFO, "Docs:: %d docs\n", docNums));
	return docNums;
}

/**
doc 정보가 화일로 저장된 경우, 화일에서 로드하기. ==> 현재 사용하지 않음.
*/
int	Docs::load_docinfo_FILE(char *fname)
{
	int	max_doc_id = 10;
	FILE	*fp;
	int	n;
	docType *doc;
	
	max_doc_id = 	10 + FileUtil::countLines(fname);
	docArray = (docType *) docmem.alloc( max_doc_id * sizeof(docType) );

	if ( (fp = fopen(fname, "r"))==NULL) return -1;

	n=0;
	while (docNums < max_doc_id) {
		char buf[256];
		
		int	fld;
		if (fgets(buf, 255,fp)==NULL) break;
		fld = n % 5;
		n++;
		//PRN("%d:%d: %s", n, fld, buf);
		if (fld==0) {
			doc = docArray + docNums;
			doc->doc_id = strtol(buf, 0, 10);
		}	
		else if (fld==1) {
			doc->url = docmem.strAlloc(buf, strlen(buf)-1);
		}
		else if (fld==2) {
			doc->fpath = docmem.strAlloc(buf, strlen(buf)-1);
		}
		else if (fld==3) {
			doc->title = docmem.strAlloc(buf, strlen(buf)-1);
			
			print(doc);
			docNums++;
		}
		
	}
	PRN("Docs:: %d docs\n", docNums);
	return docNums;
}


int	Docs::load_doctext()
{
	int	n;
	docType *doc;
	for(n=0, doc = docArray; n<docArraySize; n++, doc++) {
		int	actual_size=0;

		if (doc->doc_id <= 0) continue;
		
		if (doc->fpath) doc->size = FileUtil::get_file_size(doc->fpath);
		if ( doc->fpath==0	|| doc->size <=0) {
			ACE_DEBUG((LM_INFO, "Load Doc Text fail: ID=%d Size=%d File=%s\n",
				doc->doc_id, doc->size, doc->fpath));
			continue;
		}
		
		doc->text = (char*)docmem.alloc(doc->size);
		actual_size = FileUtil::load_file_2_given_mem(doc->text, doc->fpath, doc->size);
		ACE_DEBUG((LM_DEBUG, "%s : %d -> %d\n", doc->fpath, doc->size, actual_size));
		//PRN("%s\n%d\n", doc->text, doc->size);
	}
	return n;
}

int Docs::cmpDocList(const void *Aptr, const void *Bptr)
{
	return ((docType*)Aptr)->doc_id - ((docType*)Bptr)->doc_id;
}

int Docs::sortDocList()
{
	qsort (docArray, docNums, sizeof(docType), cmpDocList);
	return 0;
}

docType *Docs::searchDoc(int target_id)
{
	docType *doc;
	if (target_id <= 0 || target_id >= docArraySize) return NULL;
	doc = docArray + target_id;
	if (doc->doc_id == (uint4) target_id) return doc;
	else return 0;
}
/**
주어진 id를 가진 idiom 찾기.
*/
docType *Docs::searchDoc_old(int target_id)
{
	int low = 0, high = docNums - 1;
	int	cmp;
	while( low <= high )    {
		int mid = ( low + high ) / 2;
		cmp = docArray[mid].doc_id - target_id;
		if( cmp < 0 )
			low = mid + 1;
		else if( cmp > 0)
			high = mid - 1;
		else
			return &docArray[mid];   // Found
	}
	return NULL;     // NOT_FOUND 
}

void	Docs::print(docType *doc)
{
	ACE_DEBUG((LM_INFO, "DOC id=%d\nURL=%s\nFILE=%s\nTITLE=%s\nSize=%d\n", 
		doc->doc_id, doc->url, doc->fpath, doc->title, doc->size));
}
