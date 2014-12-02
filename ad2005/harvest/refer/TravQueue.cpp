//------------------ Link 관련 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "trav_q.h"

#define PRN	printf

//-----
HLINKst	hlink_list[MAX_PAGE_TRV];
int		hlinkQ_tail=0;
int		hlinkQ_head=0;
int		hlinkQ_n=0;

// (TRVexpr *trv_expr)
void TravQueue::init_hlink_trav_Q() 
{

	memset(hlink_list, 0, sizeof(HLINKst)*MAX_PAGE_TRV);
	hlinkQ_head = hlinkQ_tail = hlinkQ_n = 0;
}

void TravQueue::finish_hlink_trav_Q()
{
	PRN("-- %d links traversed\n", hlinkQ_n);	
	delete_all_hlinkQ();
	
}

int TravQueue::get_hlinkQ_size()
{
	return hlinkQ_n;
}

// return TRUE if newURL is duplicate; prevent infinite recursive fetch
int TravQueue::is_hlink_dup(URLst *newURL)
{
	int	i;
	int len;
	register HLINKst *ptr;
	//int  dir_len, dir_len2;
	//char *file_ptr;
	//int	is_index_html=0;
//	dir_len = (newURL->file)? (newURL->file - newURL->path) : strlen(newURL->path);
	if (newURL->host) {
		for(i=0, ptr=hlink_list; i<hlinkQ_tail; i++, ptr++) {
			if ( ptr->currURL.host 
				&& ptr->currURL.host_len == newURL->host_len
				&& ptr->currURL.is_index == newURL->is_index
				&& ptr->currURL.dir_len == newURL->dir_len
			  ) 
			{				
				if (newURL->is_index) len = newURL->dir_len;
				else len = newURL->path_len;
				if (
				  strncmp(ptr->currURL.path, newURL->path, len)==0
				  && strncmp(ptr->currURL.host, newURL->host, newURL->host_len )==0
				  )
					return 1;
			}
		}
		return 0;
	}
	else {
		// host가 null이면 root URL과 같은 host를 의미.
		// hlink_list 에 저장할 시, root URL과 같은 host는 null로 저장 -- 예전 방식 
		for(i=0, ptr=hlink_list; i<hlinkQ_tail; i++, ptr++) {
			if (  (i==0 || ptr->currURL.host==0)
				&& ptr->currURL.dir_len == newURL->dir_len
				&& ptr->currURL.is_index == newURL->is_index)
			{				
				if (newURL->is_index) len = newURL->dir_len;
				else len = newURL->path_len;
				if (strncmp(ptr->currURL.path, newURL->path, len)==0)
					return 1;
			}
		}
		return 0;		
	}
}

// delete from Head
HLINKst * TravQueue::get_hlinkQ_head()
{
	HLINKst *ptr;
	if (hlinkQ_head == hlinkQ_tail) return 0; // empty Q	
	ptr = hlink_list + hlinkQ_head;
	hlinkQ_head++;
	if (hlinkQ_head >= MAX_PAGE_TRV) hlinkQ_head = 0; // circular Q
	hlinkQ_n--;
	return (ptr);
}


// insert into Tail
//2002.10.2
//int insert_hlinkQ(URLst *parentURL, int curr_depth, URLst *currURL, int is_A_link)
int TravQueue::insert_hlinkQ(int curr_depth, URLst *currURL)
{
	HLINKst *new_link;
	int	next_Q_pos=0;
	next_Q_pos = (hlinkQ_tail+1);
	if (next_Q_pos >= MAX_PAGE_TRV) next_Q_pos=0;
	if (next_Q_pos == hlinkQ_head) return 0; // Q full
	
	new_link = hlink_list + hlinkQ_tail;
	hlinkQ_tail = next_Q_pos;

	hlinkQ_n++;
	new_link->curr_depth = curr_depth;
	//new_link->is_A_link = is_A_link;
	//new_link->parentURL = parentURL;

	//2003.3.21
	URL_make_dup(&(new_link->currURL), currURL);

	// A LINK's title --> NO needs
	//if (title) new_link->title = str_alloc(title);
	//else new_link->title = 0;

	return 1;
}

// delete from Head
void TravQueue::delete_all_hlinkQ()
{
	int	i;
	for(i=0; i<hlinkQ_tail; i++) {
		hlinkQ_n--;
		//if (hlink_list[i].curr_url) free(hlink_list[i].curr_url);
		//if (hlink_list[i].title) free(hlink_list[hlinkQ_head].title);
		URL_free(&(hlink_list[i].currURL));
		hlinkQ_head++;
	}
	hlinkQ_n = 0;
}

