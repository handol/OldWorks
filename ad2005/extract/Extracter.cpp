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
�־���  doc_id �� �ش��ϴ� �������� ���� ����.
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
�־��� doc_id �̻��� �����߿� processed=0 �� �������� �ϰ� ó���Ͽ� ���� ����.
���� �־��� ������ �ش��ϴ� ������ ID ����� ���� ������ {dbStore::selectDB_doc_id()}
�� ���� ���� extract_by_doc_id() ����.

fromID: �ݵ�� ���� �־����� �Ѵ�.
toID: optional ��
many: optional ��. ���� �־����� �ʴ��� �ý��� �ƽ� �� ���� �ʿ�. config�� ���� ����.
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
return 0: �Ϻ� ���� �߻�.
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
	DB �� ����� docinfo->size �� '\n' ����.
	���� ȭ�Ϸ� ����� ���� MS Windows������ '\r\n' ���� ����ȴ�.
	�� ������ �޸� ũ�� ���̷� FileUtil::load_file_2_given_mem() ���� �״� ���� �߻��Ͽ���. 
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
	/* ������ :  FileUtil::get_file_size() ���� ���� ���� load�� ���� �ٸ�. �� ���̴� ���μ��� ��ġ.  */
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
	docinfo->doc_id = 1; // extract.txt_analyze() ���� doc_id > 0 �� ��쿡�� ó���ϹǷ�.
	extract.txt_analyze(docinfo);
	extract.get_result(resStr);
	resStr.add("\n");
	
	delete [] docinfo->text;
	docinfo->text = 0;
	resStr.print();
	return 0;
	
}

/**
����� ������� �ʴ� �Լ�.
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
