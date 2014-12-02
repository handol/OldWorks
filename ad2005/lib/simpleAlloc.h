#ifndef MYMEM_H
#define MYMEM_H

/**
@file simpleAlloc.h
@brief free ���� alloc �� �ϴ� ������ memory allocation ������. 
blockBaseSize ũ���� �޸� block��  linked list�� ���Ѱ� ������ �� �ִ�.
*/

/**
�޸� block ����ü. blockBaseSize ũ���� �޸� block ����  linked list�� �����ϰ�.
*/
typedef struct _memBlockSt {
	char	*headptr; // ����� ���� ���� pointer
	char	*currptr; // free memory�� ���� pointer
	int		left; // ��� ���� ���� �޸� ũ�� (free memory size)
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
