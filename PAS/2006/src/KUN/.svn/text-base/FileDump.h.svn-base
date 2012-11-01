

#ifndef FILE_DUMP_H
#define FILE_DUMP_H

/**
@brief Packet 내용을 File로 저장
*/

#include <stdio.h>
class FileDump

{

public:
	FileDump();
	~FileDump();
	void init(const char *desc, int kind);
	void	write(const char *data, int leng);
	void	before(char *data, int orglen);
	void	after(int afterlen);
	
private:
	void get_nowtime();
	FILE *fd;
	char *beforePtr;
	int beforeLen;
	char nowtime[32];
	int	seqNum;
};

#endif
