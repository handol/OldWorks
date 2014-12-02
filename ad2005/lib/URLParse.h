#ifndef URLPARSE_H
#define URLPARSE_H

#include "basicDef.h"

typedef enum {
	unkown = 0,
	http = 1,
	https,
	mail,
	ftp,
	file,
	news
} ProtocolNum;

#define	URLPARSE_PROTOLEN	(7)
#define	URLPARSE_URLLEN	(255)
#define	URLPARSE_HOSTLEN	(31)
#define	URLPARSE_PATHLEN	(127)
#define	URLPARSE_FILELEN	(31)

/**
URL=http://www.anydic.com/aaa/bbb/yyy.cgi?aa=xx&bb=yy �� ���.
dir_len = "/aaa/bbb/" �� ����.
*/
typedef struct _URLst
{
	char	urlstr[URLPARSE_URLLEN+1]; // original URL string
	char	protocol[URLPARSE_PROTOLEN+1];
	char	host[URLPARSE_HOSTLEN+1];
	char	path[URLPARSE_PATHLEN+1]; // include directory and file name
	char	*file; 
	
	uint2	port;
	byte	prot;
	byte	country;
	byte	is_ipaddr; // TRUE if hostname is IP Address
	byte	is_index; // TRUE if index.html or null file name
	byte	host_len;
	byte	path_len;
	byte	dir_len;
	byte	file_len;	
	byte	file_head_len;
	byte	file_ext;
	byte	url_type; // host, dir, file
} URLst;

// URLst.file_ext
#define	EXT_UNDEF (0)
#define	EXT_HTML (1)
#define	EXT_TXT (2)
#define	EXT_CGI (4)
#define	EXT_PAGELINK (8) // index.html#aaa

// URLst.url_type
#define	HOST_URL (1)
#define	DIR_URL (2)
#define	FILE_URL (3)
#define	IS_HOST		(1)
#define	IS_DIR		(2)
#define	IS_FILE		(3)

// URLst.country
#define	NATION_GLOBAL	(1)
#define	NATION_CA_UK	(2)
#define	NATION_US	(4)
#define	NATION_AU_NZ	(8)

class	URLParse {
	
public:
	URLParse() {
		memset(&url, 0, sizeof(URLst) );
	}
	
	URLParse(char *org_urlstr) {
		memset(&url, 0, sizeof(URLst) );
		parse(org_urlstr);
	}

	/**
	������ URL�� ���������� link �� merge�Ͽ� ���ο� URL�� construct.
	@param parent : ���� HTML�������� URL
	@param child_link : ���� HTML ���������� <A> link�� url
	
	*/
	URLParse(URLParse &parent, URLParse &child) {
		memset(&url, 0, sizeof(URLst) );
		STRNCPY(this->url.urlstr, child.url.urlstr, URLPARSE_URLLEN);
		this->parse(NULL);
		this->merge(&parent.url);
	}

	~URLParse() {
	}

	/**
	�θ� url �� ���ڷ� �޾Ƽ�, �� url �� this URL �� merge �Ѵ�.
	*/
	int	inherit(URLParse &parent) {
		this->merge(&parent.url);
		this->parse(NULL);
		return 0;
	}

	int	isSubDir(URLParse &parent);
	
	/**
	URL�� host�ּҰ�  ���� ��� �����̸� return TRUE.

	HTTP header�� language type�� �˻��ϴ� ���� �� ��Ȯ.
	*/
	int	isEnglish(){
		return (url.country == NATION_GLOBAL || url.country == NATION_US
			|| url.country == NATION_CA_UK || url.country == NATION_AU_NZ);
	}
	int has_CGIchar(char *str);
	int has_CGIchar(char *str, int len);
	int get_fname(char *fname, char *path);
	char *get_fname(char *path);
	int get_country(char *host);
	void print();
	int parse(char *org_urlstr);
	int get_fileext();
	int trim_path(char *new_path, char *path);
	int merge(URLst *parent);

	URLst	url;
private:
	

};

#endif
