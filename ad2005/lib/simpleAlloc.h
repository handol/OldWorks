#ifndef MYMEM_H
#define MYMEM_H

/**
@file simpleAlloc.h
@brief free 없이 alloc 만 하는 간단한 memory allocation 관리자. 
blockBaseSize 크기의 메모리 block을  linked list로 무한개 생성할 수 있다.
*/

/**
메모리 block 구조체. blockBaseSize 크기의 메모리 block 들을  linked list로 관리하게.
*/
typedef struct _memBlockSt {
	char	*headptr; // 블록의 원래 시작 pointer
	char	*currptr; // free memory의 시작 pointer
	int		left; // 블록 내의 남은 메모리 크기 (free memory size)
	struct _memBlockSt *next;
} memBlockSt;


class	simpleAlloc
{
public:
	simpleAlloc() { set(10); }
	simpleAlloc(int kilo_bytes) {
		set(kilo_bytes);
	}
	~simpleAlloc() { cleanup(); }

	void set(int kilo_bytes);
	void *alloc(int requested);
	//void *alloc(char *str, int len=0);
	void *copyAlloc(void *src, int len);
	char *strAlloc(char *src, int len=0);
	void	cleanup();
	void	reuse() {
		cleanup();
		set(0);
	}
	void print();
	
private:
	
	memBlockSt *firstBlock;
	memBlockSt *currBlock;
	int	blockBaseSize;
	int	numOfBlocks;
	
	int	new_block();
};

#endif
