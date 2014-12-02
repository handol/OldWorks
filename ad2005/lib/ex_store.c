typedef struct _hExType
{
	uint4 doc_id;
	uint4 s_pos;
	uint2 s_len;
	uint2 pos1;
	uint2 len1;
	byte level;

} hExType; //한글 예문 



#include <stdio.h>
#include <stdlib.h>

#include <sys/shm.h>
#include <sys/ipc.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define PRN	printf

// 1 block == 10 Megabytes
#define	BLOCK_SIZE	(1024*1024*10)
#define	BUFF_SIZE	(1024*8)

int load_mem_from_file(char *mem_ptr, int size, char fname)
{
	int	block;
	int	nbytes, nread=0;
	int	fd;
	
	if ( (fd = open(fname, O_RDONLY)) < 0) {
		printf("File Read Fail : %s\n", fname);
		return -1;
	}

	for(nread=0; nread < BLOCK_SIZE; ) {
		nbytes= read(fd, mem_ptr, BUFF_SIZE);
		if (nbytes > 0) {
			nread += nbytes;
			mem_ptr += nbytes;
		}					
		if (nbytes < BUFF_SIZE) break;	
	}

	printf("Read: %s - %d Kb\n", fname, (nread >> 10) );
	close(fd);
	
}

int dump_mem_to_file(char *mem_ptr, int size, char *fname)
{
	int	block;
	int	nbytes, nwrite=0;
	int	fd;

	if ( (fd = creat(fname, 0644)) < 0) {
		printf("File Write Fail : %s\n", fname);
		return -1;
	}

	for(nwrite=0; nwrite < BLOCK_SIZE;) {
		nbytes= write(fd, mem_ptr, BUFF_SIZE);
		if (nbytes > 0) {
			nwrite += nbytes;
			mem_ptr += nbytes;
		}					
		if (nbytes < BUFF_SIZE) break;	
	}

	printf("Written: %s - %d Kb\n", fname, (nwrite >> 10) );
	close(fd);
}

int append_mem_to_file(char *mem_ptr, int size, char *fname)
{
	int	block;
	int	nbytes, nwrite=0;
	int	fd;

	if ( (fd = open(fname, O_APPEND)) < 0) {
		if ( (fd = creat(fname, 0644)) < 0) {
			printf("File Write Fail : %s\n", fname);
			return -1;
		}
		printf("File Read Fail : %s\n", fname);
	}
	
	for(nwrite=0; nwrite < BLOCK_SIZE;) {
		nbytes= write(fd, mem_ptr, BUFF_SIZE);
		if (nbytes > 0) {
			nwrite += nbytes;
			mem_ptr += nbytes;
		}					
		if (nbytes < BUFF_SIZE) break;	
	}

	printf("Written: %s - %d Kb\n", fname, (nwrite >> 10) );
	close(fd);
}
