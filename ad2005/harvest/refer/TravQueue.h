#ifndef HLINK_Q_H
#define HLINK_Q_H

#include "basic_def.h"
#include "url_data.h"


typedef struct _hyperlink_st {
	byte	curr_depth;
	URLst	currURL;
} HLINKst;

class TravQueue {
public:
	
void init_hlink_trav_Q();
void finish_hlink_trav_Q();
int is_hlink_dup(URLst *newURL);
HLINKst * get_hlinkQ_head();
//int insert_hlinkQ(URLst *parentURL, int curr_depth, URLst *currURL, int is_A_link);
int insert_hlinkQ(int curr_depth, URLst *currURL);
void delete_all_hlinkQ();
};
#endif
