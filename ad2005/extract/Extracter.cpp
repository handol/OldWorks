#include "ace/Log_Msg.h"
#include "Extracter.h"
#include "Spliter.h"
#include "CGI.h"
#include "FileUtil.h"
#include "StrUtil.h"

#include "EDict.h"
#include "Docs.h"
#include "TextProc.h"
#include "dbStore.h"
#include "FuncLogger.h"

/**
주어진  doc_id 에 해당하는 문서에서 예문 추출.
*/
int	Extracter::extract_by_docid(int docid, StrStream &resStr)
{
	FuncLogger log("extract_by_docid");
	docType docinfo;
	char	url[256]={0}, fname[256]={0}, title[256]={0};
		
	memset(&docinfo, 0, sizeof(docType) );
	
	docinfo.doc_id = docid;

	docinfo.url = url;
	docinfo.title = title;
	docinfo.fpath = fname;

	if ( dbStore::readDB_doc(&docinfo) < 0) {
		resStr.add("DB read fail: doc_id = %d\n\n", docinfo.doc_id);
		return -2;
	}
	
	return do_extract(&docinfo, resStr);
}


/**
주어진 doc_id 이상의 문서중에 processed=0 인 문서들을 일괄 처리하여 예문 추출.
먼저 주어진 조걵에 해당하는 문서의 ID 목록을 구한 다음에 {dbStore::selectDB_doc_id()}
각 문서 별로 extract_by_doc_id() 수행.

fromID: 반드시 값이 주어져야 한다.
toID: optional 값
many: optional 값. 값이 주어지지 않더라도 시스템 맥스 값 설정 필요. config로 설정 가능.
*/
#define	MAX_EXTRACT_DOCS	(10)

int	Extracter::extract_range(int fromID, int toID, int many, StrStream &resStr)
{
	FuncLogger log("extract_range");
	int	*docid_list;
	int	list_size = MAX_EXTRACT_DOCS;
	int	doc_cnt =0;
	int	i=0;
	if (many)
	list_size = MIN(many, list_size);
	if (toID) {
		int range = (toID-fromID+1);
		list_size = MIN(range, list_size);
	}
	if (list_size <= 0) return 0;

	
	docid_list = new int[list_size*2];
	if (docid_list==0) {
		resStr.add("$ERROR$ Memory fail\n");
		return 0;
	}

	doc_cnt = dbStore::selectDB_doc_id(docid_list, list_size, fromID, toID);

	resStr.add("$NUMDOCS$ %d\n", doc_cnt);
	if (doc_cnt > 0) resStr.add("$DOCIDS$ ");
	
	for(i=0; i<doc_cnt; i++) {
		resStr.add("%d ", docid_list[i]);
	}
	resStr.add("\n\n");
	
	for(i=0; i<doc_cnt; i++) {
		extract_by_docid (docid_list[i], resStr);
	}
	
	delete [] docid_list;
	return doc_cnt;
}

/**
return 1: if processed and updated 
return 0: 일부 오류 발생.
*/
int	Extracter::do_extract(docType *docinfo, StrStream &resStr)
{
	FuncLogger log("do_extract");
	TextProc	extract;
	int	alloc_size=0;
	int	load_size=0;

	if (docinfo->processed != 0) {
		resStr.add("$SKIP$ %d , processed (%d)\n", docinfo->doc_id, docinfo->processed);
		return 0;
	}
	
	resStr.add("$DOCID$ %d\n$URL$ %s\n", docinfo->doc_id, docinfo->url);
	resStr.add("File=%s\nTitle=%s\n", docinfo->fpath,  docinfo->title);

	/*
	DB 에 저장된 docinfo->size 는 '\n' 기준.
	실제 화일로 저장될 때는 MS Windows에서는 '\r\n' 으로 저장된다.
	이 때문에 메모리 크기 차이로 FileUtil::load_file_2_given_mem() 에서 죽는 문제 발생하였음. 
	*/
	docinfo->size = FileUtil::get_file_size(docinfo->fpath);

	ACE_DEBUG ((LM_DEBUG,"[%T] DOCID %d, %s, Size=%d\n", docinfo->doc_id, docinfo->fpath, docinfo->size));
	
	if (docinfo->size <= 0) {
		return 0;
	}
	
	alloc_size = ALIGN_HALF_K ( docinfo->size );

	
	docinfo->text = new char[ alloc_size ];
	ACE_DEBUG ((LM_DEBUG, "alloc_size=%d\n", alloc_size));
	
	load_size = FileUtil::load_file_2_given_mem(docinfo->text, docinfo->fpath, docinfo->size);
	/* 실행결과 :  FileUtil::get_file_size() 에서 얻은 값과 load한 값이 다름. 그 차이는 라인수와 일치.  */
	ACE_DEBUG ((LM_DEBUG, "fsize in DB = %d, loaded size = %d\n", docinfo->size, load_size));

	docinfo->size = load_size;
	
	TextProc::prepare();

	extract.setDebug(1);
	extract.txt_analyze(docinfo);
	extract.get_result(resStr);
	resStr.add("\n");
	
	if (docinfo->text)
		delete [] docinfo->text;
	docinfo->text = 0;
	
	docinfo->processed = 1;
	if ( dbStore::updateDB_doc(docinfo) < 0) {
		resStr.add("DB update fail: DOCID = %d\n\n", docinfo->doc_id);
		return 0;
	}
	
	return 1;
}

int Extracter::test_ExtractOne(char *text)
{
	docType *docinfo = new docType();
	char	fullname[256];

	TextProc	extract;
	StrStream resStr;
	resStr.init(1024);

	strcpy(fullname, "NOFILE-test");
	docinfo->fpath = fullname;

	int text_len = strlen(text);
	int alloc_size = ALIGN_HALF_K ( text_len );

	docinfo->text = new char[ alloc_size ];
	
	strcpy(docinfo->text, text);
	docinfo->size = text_len;

	resStr.add("# %d file bytes, %d mem bytes, %s\n", docinfo->size, alloc_size, docinfo->fpath);
	
	TextProc::prepare();

	extract.setDebug(0);
	docinfo->doc_id = 1; // extract.txt_analyze() 에서 doc_id > 0 인 경우에만 처리하므로.
	extract.txt_analyze(docinfo);
	extract.get_result(resStr);
	resStr.add("\n");
	
	delete [] docinfo->text;
	docinfo->text = 0;
	resStr.print();
	return 0;
	
}

/**
현재는 사용하지 않는 함수.
*/
void	Extracter::extract_by_fname(char *textfname, StrStream &resStr)
{
	char *textbuff;
	int	textlen=0;
	TextProc	extract;
	textbuff = new char[1024*100];
	docType docinfo;
	
	textlen = FileUtil::load_file_2_given_mem(textbuff, textfname, 1024*100);
	printf("%d\n", textlen);
	//printf("%s\n", textbuff);

	if (textlen > 0) {
		TextProc::prepare();
		docinfo.text = textbuff;
		docinfo.size = textlen;
		extract.txt_analyze(&docinfo);
		extract.get_result( resStr);
	}
	delete [] textbuff;

}

int Extracter::load_file_2_given_mem(char *mem, char *fname, int max_size)
{
	FILE *fp;
	char buf[1024];
	int	n=0;

	mem[0] = 0;
	if ( (fp=fopen(fname, "r")) == NULL) return -1;
	while (n < max_size && fgets(buf, 1024, fp) != NULL) {		
		strcpy(mem+n, buf);
		n += strlen(buf);
		//ACE_DEBUG ((LM_DEBUG, "%d\n", n));
	}	
	mem[n] = 0;
	fclose(fp);
	
	return n;
}
